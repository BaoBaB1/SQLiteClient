#pragma once

#include "sqlite3.h"
#include "Singleton.h"
#include "Noncopyable.h"
#include "Cache.h"
#include <cassert>
#include <optional>
#include <vector>
#include <typeinfo>

class DbEntity;

class Session : public Noncopyable
{
public:
  static Session& instance();

  bool insert(DbEntity*);
  bool remove(DbEntity*);
  bool update(DbEntity*); 
  template<typename T, typename std::enable_if<std::is_base_of_v<DbEntity, T>, bool>::type = true >
  bool update(T& entity);
  template<typename T, typename std::enable_if<std::is_base_of_v<DbEntity, T>, bool>::type = true >
  std::optional<T> get(size_t id);
  template<typename T, typename std::enable_if<std::is_base_of_v<DbEntity, T>, bool>::type = true >
  std::vector<T> all(const char* sql = nullptr);

  int execute(const char* sql, int (*callback)(void*, int, char**, char**), void*);
  void close();
  sqlite3* connection() const { return m_conn; }
  friend class Singleton<Session>;
private:
  Session();
private:
  sqlite3* m_conn;
};

template<typename T, typename std::enable_if<std::is_base_of_v<DbEntity, T>, bool>::type >
bool Session::update(T& entity)
{
  // Always use Session::update() instead of direct DbEntity::update_in_db() since it updates cache
  Cache<T>::remove(entity.id());
  return entity.update_in_db(*this);
}

template<typename T, typename std::enable_if<std::is_base_of_v<DbEntity, T>, bool>::type >
std::optional<T> Session::get(size_t id)
{
  if (Cache<T>::is_in_cache(id))
    return *Cache<T>::get(id);
  T t;
  if (t.load_from_db(*this, id))
  {
    Cache<T>::cache(t);
    return std::optional<T>(t);
  }
  return std::nullopt; 
}

template<typename T, typename std::enable_if<std::is_base_of_v<DbEntity, T>, bool>::type >
std::vector<T> Session::all(const char* sql)
{
  std::string query("SELECT * FROM ");
  query.append(T().table());
  if (sql)
    query.append(sql);
  std::vector<int> ids;
  execute(query.c_str(), count_callback_with_ids, &ids);
  std::vector<T> res(ids.size());
  std::string name; 
  for (size_t i = 0; i < ids.size(); i++)
  {
    if (Cache<T>::is_in_cache(ids[i]))
    {
      res[i] = *Cache<T>::get(ids[i]);
    }
    else
    {
      T t;
      t.load_from_db(*this, ids[i]);
      Cache<T>::cache(t);
      res[i] = std::move(t);
    }
  }
  return res;
}
