#include "Customer.h"
#include "../utils/Utils.h"

static constexpr std::array<std::string_view, 7> g_fields = { "id", "name", "age", "phone", "country", "city", "address" };
static int read_callback(void* first, int argc, char** argv, char** col);

size_t Customer::hash() const
{
  std::hash<std::string> str_hasher;
  std::hash<unsigned> u_hasher;
  size_t hash = str_hasher(m_name) + str_hasher(m_phone) + 
    str_hasher(m_country) + str_hasher(m_city) + str_hasher(m_address);
  hash += u_hasher(m_age);
  hash += DbEntity::hash();
  return hash;
}

bool Customer::save_in_db(Session& s)
{
  using string = std::string;
  std::tuple<string, unsigned, string, string, string, string> values(m_name, m_age, m_phone, m_country, m_city, m_address);
  return DbEntity::save(s, g_fields, values);
}

bool Customer::load_from_db(Session& s, size_t id) 
{
  return DbEntity::load(s, id, read_callback, this);
}

bool Customer::remove_from_db(Session& s)
{
  return DbEntity::remove(s);
}

bool Customer::update_in_db(Session& s)
{
  using string = std::string;
  std::tuple<string, int, string, string, string, string> values(m_name, m_age, m_phone, m_country, m_city, m_address);
  return DbEntity::update(s, g_fields, values);
}

std::vector<std::string> Customer::fields() const
{
  return std::vector<std::string>(g_fields.begin(), g_fields.end());
}

std::vector<std::string> Customer::values() const
{
  std::vector<std::string> v(g_fields.size());
  v[0] = std::to_string(id());
  v[1] = m_name;
  v[2] = std::to_string(m_age);
  v[3] = m_phone;
  v[4] = m_country;
  v[5] = m_city;
  v[6] = m_address;
  return v;
}

bool Customer::fill_with(const std::vector<std::string>& fields, const std::vector<std::string>& values)
{
  bool ok = true;
  Customer tmp(*this);
  for (int i = 0; i < (int)fields.size(); i++)
  {
    if (fields[i] == "id")
    {
      ok = read_id_callback(this, (char*)(values[i].c_str()));
      if (!ok) break;
    }
    else if (fields[i] == "name")
    {
      m_name = values[i];
    }
    else if (fields[i] == "age")
    {
      ok = read_into(values[i], &m_age);
      if (!ok) break;
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
  Customer* cust = static_cast<Customer*>(first);
  std::vector<std::string> fields(argc), values(argc);
  for (int i = 0; i < argc; ++i)
  {
    fields[i] = cols[i];
    values[i] = argv[i];
  }
  assert(cust->fill_with(fields, values));
  return 0;
}
