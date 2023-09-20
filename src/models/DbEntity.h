#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <string_view>
#include <vector>
#include <tuple>
#include <array>
#include "../core/Session.h"

// callback to set id of entity after SQLite query (insert, select)
static bool read_id_callback(void* ptr, char* argv);
static int count_callback_with_ids(void* p_vec_int, int argc, char** argv, char** col_name);

class IDbEntity
{
public:
  virtual size_t hash() const = 0;
  virtual bool save_in_db(Session&) = 0;
  virtual bool load_from_db(Session&, size_t) = 0;
  virtual bool remove_from_db(Session&) = 0;
  virtual bool update_in_db(Session&) = 0;
  virtual std::vector<std::string> fields() const = 0;
  virtual std::vector<std::string> values() const = 0;
  virtual bool fill_with(const std::vector<std::string>& fields, const std::vector<std::string>& values) = 0;
  virtual ~IDbEntity() = default;
};

class DbEntity : public IDbEntity
{
public:
  enum Type
  {
    eCustomer = 1,
    eOrder,
    eSupplier,
    eProduct,
    eOrderProduct
  };
public:
  size_t hash() const override { std::hash<size_t> h; return h(m_id); }
  bool is_valid_id() const { return m_id != (size_t)(-1); }
  size_t id() const { return m_id; }
  const char* table() const { return m_table; }
  Type type() const { return m_type; }
  friend bool read_id_callback(void* ptr, char* argv);
protected:
  DbEntity(const char* table, Type type) : m_id(-1), m_table(table), m_type(type) {}
  // non public CRUD
  template<int N, typename... Ts>
  bool save(Session&, const std::array<std::string_view, N>& fields, const std::tuple<Ts...>& values);
  template<int N, typename... Ts>
  bool update(Session&, const std::array<std::string_view, N>& fields, const std::tuple<Ts...>& values);
  bool load(Session&, size_t, int (*callback)(void*, int, char**, char**), void* first);
  bool remove(Session&);
private:
  const char* m_table;
  Type m_type;
  size_t m_id;
};

template<int N, typename... Ts>
bool DbEntity::save(Session& s, const std::array<std::string_view, N>& fields, const std::tuple<Ts...>& values)
{
  if (fields.empty())
    return false;
  // minus "id" because id will be assigned during insertion
  assert(std::string(fields[0]) == "id" && N - 1 == sizeof...(Ts));
  std::string sql_insert;
  sql_insert.reserve(256);
  sql_insert.append("INSERT INTO ").append(m_table).append(" (");
  // skip field "id"
  for (size_t i = 1; i < fields.size(); i++)
  {
    if (i < fields.size() - 1)
      sql_insert = sql_insert + fields[i].data() + ",";
    else
      sql_insert += fields[i].data();
  }
  sql_insert += ") VALUES (";
  std::vector<std::string> fields_as_sql(sizeof...(Ts));
  fields_to_sql(values, fields_as_sql);
  for (size_t i = 0; i < fields_as_sql.size(); i++)
  {
    sql_insert += fields_as_sql[i];
    if (i < fields_as_sql.size() - 1)
      sql_insert += ",";
  }
  sql_insert += ");";
  std::cout << "SQL insert query: " << sql_insert << '\n';
  bool ok = s.execute(sql_insert.c_str(), nullptr, nullptr) == SQLITE_OK;
  if (ok)
    m_id = sqlite3_last_insert_rowid(s.connection());
  return ok;
}

template<int N, typename... Ts>
bool DbEntity::update(Session& s, const std::array<std::string_view, N>& fields, const std::tuple<Ts...>& values)
{
  if (fields.empty())
    return false;
  // N - 1 cuz "id" is present in 'fields' but absent in 'values'
  assert(fields[0] == "id" && N - 1 == sizeof...(Ts));
  if (is_valid_id())
  {
    std::string sql_update;
    sql_update.reserve(256);
    sql_update.append("UPDATE ").append(m_table).append(" SET ");
    std::vector<std::string> fields_as_sql(sizeof...(Ts));
    fields_to_sql(values, fields_as_sql);
    for (size_t i = 1; i < fields.size(); i++)
    {
      sql_update.append(fields[i]).append(" = ").append(fields_as_sql[i - 1]);
      if (i < fields.size() - 1)
        sql_update += ", ";
    }
    sql_update.append(" WHERE id = ").append(std::to_string(m_id)).append(";");
    std::cout << "SQL update query: " << sql_update << '\n';
    return s.execute(sql_update.c_str(), nullptr, nullptr) == SQLITE_OK;
  }
  return false;
}

template<size_t I, typename... Args>
typename std::enable_if< I < sizeof...(Args), std::string>::type field_to_sql(const std::tuple<Args...>& tup)
{
  // enable_if I less than count of elements in the tuple
  return ::to_sql_string(std::get<I>(tup));
}

template<size_t I = 0, typename... Args>
typename std::enable_if < I == sizeof...(Args), void>::type fields_to_sql(const std::tuple<Args...>& tup, std::vector<std::string>& fields)
{
  // I == count of elements in the tuple
}

template<size_t I = 0, typename... Args>
typename std::enable_if< I < sizeof...(Args), void>::type fields_to_sql(const std::tuple<Args...>& tup, std::vector<std::string>& fields)
{
  fields[I] = std::move(field_to_sql<I>(tup));
  fields_to_sql<I + 1>(tup, fields);
}

template<typename T> 
std::string to_sql_string(const T& t)
{
  return std::to_string(t);
}

template<>
inline std::string to_sql_string<std::string>(const std::string& s)
{
  std::string tmp(s.size() + 2, '#');
  // string in double single quotes
  tmp[0] = tmp[tmp.size() - 1] = '\'';
  for (size_t i = 0; i < s.size(); i++)
    tmp[i + 1] = s[i];
  return tmp;
}

template<typename T> 
bool read_into(const std::string& from, T* into)
{
  T tmp(*into);
  std::stringstream ss(from);
  ss >> *into;
  if (ss.fail())
  {
    *into = tmp;
    return false;
  }
  return true;
}

static bool read_id_callback(void* ptr, char* argv)
{
  // special case when adding new entity our id field is empty 
  if (std::string(argv) == "")
    return true;
  DbEntity* p = static_cast<DbEntity*>(ptr);
  return read_into(argv, &p->m_id);
}

static int count_callback_with_ids(void* p_vec_int, int argc, char** argv, char** col_name)
{
  std::vector<int>* v = static_cast<std::vector<int>*>(p_vec_int);
  for (int i = 0; i < argc; i++)
  {
    std::string col(col_name[i]);
    if (col == "id")
      v->push_back(std::stoi(argv[i]));
  }
  return 0;
}
