#include <iostream>
#include <stdexcept>
#include "Session.h"
#include "../models/Customer.h"
#include "../models/Order.h"
#include "../models/OrderProduct.h"
#include "../models/Supplier.h"
#include "../models/Product.h"

Session& Session::instance()
{
  return Singleton<Session>::instance();
}

Session::Session()
{
  if (sqlite3_open("../ShopManagementSystem.db", &m_conn) != SQLITE_OK)
  {
    std::string msg;
    msg.append("Can't open database ").append(sqlite3_errmsg(m_conn)).append("\n");
    std::cerr << msg;
    sqlite3_close(m_conn);
    throw std::runtime_error(msg);
  }
  // allow FKs and their cascades
  execute("PRAGMA foreign_keys = ON;", nullptr, nullptr);
}

bool Session::insert(DbEntity* p)
{
  return p->save_in_db(*this);
}

bool Session::remove(DbEntity* p)
{
  return p->remove_from_db(*this);
}

bool Session::update(DbEntity* p)
{
  DbEntity::Type type = p->type();
  switch (type)
  {
  case DbEntity::eCustomer:
    return update<Customer>(static_cast<Customer&>(*p));
  case DbEntity::eOrder:
    return update<Order>(static_cast<Order&>(*p));
  case DbEntity::eOrderProduct:
    return update<OrderProduct>(static_cast<OrderProduct&>(*p));
  case DbEntity::eSupplier:
    return update<Supplier>(static_cast<Supplier&>(*p));
  case DbEntity::eProduct:
    return update<Product>(static_cast<Product&>(*p));
  default:
    return false;
  }
}

void Session::close()
{
  sqlite3_close(m_conn);
}

int Session::execute(const char* sql, int (*callback)(void*, int, char**, char**), void* first_callback_arg)
{
  char* error;
  int rc = sqlite3_exec(m_conn, sql, callback, first_callback_arg, &error);
  if (rc != SQLITE_OK)
  {
    printf("SQL query \'%s\' error: %s\n", sql, error);
    sqlite3_free(error);
  }
  return rc;
}
