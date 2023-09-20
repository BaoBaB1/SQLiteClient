#include "Utils.h"

std::string utils::type_to_string(DbEntity::Type type)
{
  std::string str;
  switch (type)
  {
  case DbEntity::Type::eCustomer:
    str = "Customer";
    break;
  case DbEntity::Type::eOrder:
    str = "Order";
    break;
  case DbEntity::Type::eSupplier:
    str = "Supplier";
    break;
  case DbEntity::Type::eProduct:
    str = "Product";
    break;
  case DbEntity::Type::eOrderProduct:
    str = "Order product";
    break;
  }
  return str;
}
