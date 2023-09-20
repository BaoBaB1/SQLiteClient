#pragma once

#include <list>
#include <map>

static constexpr int MAX_SIZE = 100;

// LRU
template<typename T>
class Cache 
{
  using list_iter = typename std::list<T>::iterator;
public:
  static T* get(size_t id);
  static bool is_in_cache(size_t id);
  static void cache(const T& val);
  static bool remove(size_t id);
  static void clear();
private:
  static std::list<T> m_queue;
  static std::map<size_t, list_iter> m_mp;
};

template<typename T> 
T* Cache<T>::get(size_t id)
{
  if (is_in_cache(id))
  {
    // move to the head in constant time
    m_queue.splice(m_queue.begin(), m_queue, m_mp[id]);
    return &(*m_mp[id]);
  }
  return nullptr;
}

template<typename T>
bool Cache<T>::is_in_cache(size_t id)
{
  return m_mp.find(id) != m_mp.end();
}

template<typename T>
void Cache<T>::cache(const T& val)
{
  if (m_mp.size() == MAX_SIZE)
  {
    T* last = &m_queue.back();
    remove(last->id());
  }
  m_queue.push_front(val);
  m_mp[val.id()] = m_queue.begin();
}

template<typename T>
bool Cache<T>::remove(size_t id)
{
  auto it = m_mp.find(id);
  if (it != m_mp.end())
  {
    list_iter iter = m_mp[id];
    m_queue.erase(iter);
    m_mp.erase(id);
    return true;
  }
  return false;
}

template<typename T>
void Cache<T>::clear()
{
  m_mp.clear();
}

template<typename T>
std::map<size_t, typename Cache<T>::list_iter> Cache<T>::m_mp;

template<typename T>
std::list<T> Cache<T>::m_queue;
