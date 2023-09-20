#pragma once

#include <string>
#include <sstream>
#include <memory>
#include "../models/DbEntity.h"

namespace utils
{
  template<typename T>
  std::string to_string_with_precision(T v, int prec = 2)
  {
    std::ostringstream oss;
    oss.precision(prec);
    oss << std::fixed << v;
    return oss.str();
  }

  std::string type_to_string(DbEntity::Type type);
  
}
