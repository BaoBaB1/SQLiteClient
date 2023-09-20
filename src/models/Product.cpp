#include "Product.h"
#include "../utils/Utils.h"

static constexpr std::array<std::string_view, 5> g_fields = { "id", "supplierId", "name", "price", "discount" };
static int read_callback(void* first, int argc, char** argv, char** col);

size_t Product::hash() const
{
  std::hash<size_t> ull_hasher;
  std::hash<float> f_hasher;
  std::hash<std::string> str_hasher;
  size_t hash = ull_hasher(m_supplier_id);
  hash += str_hasher(m_name);
  hash += f_hasher(m_price) + f_hasher(m_discount);
  hash += DbEntity::hash();
  return hash;
}

bool Product::save_in_db(Session& s)
{
  std::tuple<size_t, std::string, float, float> values(m_supplier_id, m_name, m_price, m_discount);
  return DbEntity::save(s, g_fields, values);
}

bool Product::load_from_db(Session& s, size_t id)
{
  return DbEntity::load(s, id, read_callback, this);
}

bool Product::remove_from_db(Session& s)
{
  return DbEntity::remove(s);
}

bool Product::update_in_db(Session& s)
{
  std::tuple<size_t, std::string, float, float> values(m_supplier_id, m_name, m_price, m_discount);
  return DbEntity::update(s, g_fields, values);
}

std::vector<std::string> Product::fields() const
{
  return std::vector<std::string>(g_fields.begin(), g_fields.end());
}

std::vector<std::string> Product::values() const
{
  std::vector<std::string> v(g_fields.size());
  v[0] = std::to_string(id());
  v[1] = std::to_string(m_supplier_id);
  v[2] = m_name;
  v[3] = utils::to_string_with_precision(m_price);
  v[4] = utils::to_string_with_precision(m_discount);
  return v;
}

bool Product::fill_with(const std::vector<std::string>& fields, const std::vector<std::string>& values)
{
  bool ok = true;
  Product tmp(*this);
  for (int i = 0; i < (int)fields.size(); i++)
  {
    if (fields[i] == "id")
    {
      ok = read_id_callback(this, (char*)values[i].c_str());
      if (!ok) break;
    }
    else if (fields[i] == "supplierId")
    {
      ok = read_into(values[i], &m_supplier_id);
      if (!ok) break;
    }
    else if (fields[i] == "name")
    {
      m_name = values[i];
    }
    else if (fields[i] == "price")
    {
      ok = read_into(values[i], &m_price);
      if (!ok) break;
    }
    else if (fields[i] == "discount")
    {
      ok = read_into(values[i], &m_discount);
      if (!ok) break;
    }
  }
  if (!ok)
    *this = tmp;
  return ok;
}

static int read_callback(void* first, int argc, char** argv, char** cols)
{
  Product* prod = static_cast<Product*>(first);
  std::vector<std::string> fields(argc), values(argc);
  for (int i = 0; i < argc; i++)
  {
    fields[i] = cols[i];
    values[i] = argv[i];
  }
  assert(prod->fill_with(fields, values));
  return 0;
}