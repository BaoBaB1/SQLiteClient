#include "Supplier.h"

static constexpr std::array<std::string_view, 6> g_fields = { "id", "name", "phone", "country", "city", "address" };
static int read_callback(void* first, int argc, char** argv, char** col);

size_t Supplier::hash() const
{
  std::hash<std::string> str_hasher;
  size_t hash = str_hasher(m_name) + str_hasher(m_phone) + 
    str_hasher(m_country) + str_hasher(m_city) + str_hasher(m_address);
  hash += DbEntity::hash();
  return hash;
}

bool Supplier::save_in_db(Session& s)
{
  using string = std::string;
  std::tuple<string, string, string, string, string> values(m_name, m_phone, m_country, m_city, m_address);
  return DbEntity::save(s, g_fields, values);
}

bool Supplier::load_from_db(Session& s, size_t id)
{
  return DbEntity::load(s, id, read_callback, this);
}

bool Supplier::remove_from_db(Session& s)
{
  return DbEntity::remove(s);
}

bool Supplier::update_in_db(Session& s)
{
  using string = std::string;
  std::tuple<string, string, string, string, string> values(m_name, m_phone, m_country, m_city, m_address);
  return DbEntity::update(s, g_fields, values);
}

std::vector<std::string> Supplier::fields() const
{
  return std::vector<std::string>(g_fields.begin(), g_fields.end());
}

std::vector<std::string> Supplier::values() const
{
  std::vector<std::string> v(g_fields.size());
  v[0] = std::to_string(id());
  v[1] = m_name;
  v[2] = m_phone;
  v[3] = m_country;
  v[4] = m_city;
  v[5] = m_address;
  return v;
}

bool Supplier::fill_with(const std::vector<std::string>& fields, const std::vector<std::string>& values)
{
  bool ok = true;
  Supplier tmp(*this);
  for (int i = 0; i < (int)fields.size(); i++)
  {
    if (fields[i] == "id")
    {
      ok = read_id_callback(this, (char*)values[i].c_str());
      if (!ok) break;
    }
    else if (fields[i] == "name")
    {
      m_name = values[i];
    }
    else if (fields[i] == "phone")
    {
      m_phone = values[i];
    }
    else if (fields[i] == "country")
    {
      m_country = values[i];
    }
    else if (fields[i] == "city")
    {
      m_city = values[i];
    }
    else if (fields[i] == "address")
    {
      m_address = values[i];
    }
  }
  if (!ok)
    *this = tmp;
  return ok;
}

static int read_callback(void* first, int argc, char** argv, char** cols)
{
  Supplier* sup = static_cast<Supplier*>(first);
  std::vector<std::string> fields(argc), values(argc);
  for (int i = 0; i < argc; i++)
  {
    fields[i] = cols[i];
    values[i] = argv[i];
  }
  assert(sup->fill_with(fields, values));
  return 0;
}
