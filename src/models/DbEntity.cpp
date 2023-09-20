#include "DbEntity.h"
#include <cassert>

bool DbEntity::load(Session& s, size_t id, int (*callback)(void*, int, char**, char**), void* first)
{
  std::string sql = std::string("SELECT * FROM ") + m_table + " WHERE ID = " + std::to_string(id) + ";";
  std::cout << "SQL select query: " << sql << '\n';
  bool ok = s.execute(sql.c_str(), callback, first) == SQLITE_OK;
  return ok && is_valid_id();
}

bool DbEntity::remove(Session& s)
{
  if (is_valid_id())
  {
    std::string sql_delete = std::string("DELETE FROM ") + m_table + " WHERE ID = " + std::to_string(m_id) + ";";
    std::cout << "SQL select query: " << sql_delete << '\n';
    return s.execute(sql_delete.c_str(), nullptr, nullptr) == SQLITE_OK;
  }
  return false;
}
