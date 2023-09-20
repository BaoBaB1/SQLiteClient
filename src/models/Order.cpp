#include "Order.h"
#include "../utils/Utils.h"

static constexpr std::array<std::string_view, 5> g_fields = { "id", "customerId", "totalPrice", "deliveryAddress", "status" };
static int read_callback(void* ptr, int argc, char** argv, char** col);

size_t Order::hash() const
{
  std::hash<std::string> str_hasher;
  std::hash<size_t> u_hasher;
  std::hash<float> f_hasher;
  size_t hash = str_hasher(m_delivery_address) + str_hasher(m_status);
  hash += u_hasher(m_customer_id);
  hash += f_hasher(m_total_price);
  hash += DbEntity::hash();
  return hash;
}

bool Order::save_in_db(Session& s)
{
  std::tuple<size_t, float, std::string, std::string> values(m_customer_id, m_total_price, m_delivery_address, m_status);
  return DbEntity::save(s, g_fields, values);
}

bool Order::load_from_db(Session& s, size_t id)
{
  return DbEntity::load(s, id, read_callback, this);
}

bool Order::remove_from_db(Session& s)
{
  return DbEntity::remove(s);
}

bool Order::update_in_db(Session& s)
{
  std::tuple<size_t, float, std::string, std::string> values(m_customer_id, m_total_price, m_delivery_address, m_status);
  return DbEntity::update(s, g_fields, values);
}

std::vector<std::string> Order::fields() const
{
  return std::vector<std::string>(g_fields.begin(), g_fields.end());
}

std::vector<std::string> Order::values() const
{
  std::vector<std::string> v(g_fields.size());
  v[0] = std::to_string(id());
  v[1] = std::to_string(m_customer_id);
  v[2] = utils::to_string_with_precision(m_total_price);
  v[3] = m_delivery_address;
  v[4] = m_status;
  return v;
}

bool Order::fill_with(const std::vector<std::string>& fields, const std::vector<std::string>& values)
{
  bool ok = true; 
  Order tmp(*this);
  for (int i = 0; i < (int)fields.size(); i++)
  {
    if (fields[i] == "id")
    {
      ok = read_id_callback(this, (char*)values[i].c_str());
      if (!ok) break;
    }
    else if (fields[i] == "customerId")
    {
      ok = read_into(values[i], &m_customer_id);
      if (!ok) break;
    }
    else if (fields[i] == "totalPrice")
    {
      ok = read_into(values[i], &m_total_price);
      if (!ok) break;
    }
    else if (fields[i] == "deliveryAddress")
    {
      m_delivery_address = values[i];
    }
    else if (fields[i] == "status")
    {
      m_status = values[i];
    }
  }
  if (!ok)
    *this = tmp;
  return ok;
}

static int read_callback(void* ptr, int argc, char** argv, char** cols)
{
  Order* order = static_cast<Order*>(ptr);
  std::vector<std::string> fields(argc), values(argc);
  for (int i = 0; i < argc; i++)
  {
    fields[i] = cols[i];
    values[i] = argv[i];
  }
  assert(order->fill_with(fields, values));
  return 0;
}