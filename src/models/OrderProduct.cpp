#include "OrderProduct.h"
#include "Order.h"

static constexpr std::array<std::string_view, 4> g_fields = { "id", "productId", "orderId", "quantity" };
static int read_callback(void* first, int argc, char** argv, char** col);

size_t OrderProduct::hash() const
{
  std::hash<size_t> ull_hasher;
  std::hash<unsigned> u_hasher;
  size_t hash = ull_hasher(m_product_id) + ull_hasher(m_order_id);
  hash += u_hasher(m_quantity);
  hash += DbEntity::hash();
  return hash;
}

bool OrderProduct::load_from_db(Session& s, size_t id)
{
  return DbEntity::load(s, id, read_callback, this);
}

// OrderProduct table has triggers on update, delete, insert which update Order.totalPrice accordingly.
// To see these changes we have to remove Order from cache with Order.id == old.OrderProduct.orderId && new.OrderProduct.orderId
bool OrderProduct::save_in_db(Session& s)
{
  std::tuple<size_t, size_t, unsigned> values(m_product_id, m_order_id, m_quantity);
  Cache<Order>::remove(m_order_id);
  return DbEntity::save(s, g_fields, values);
}

bool OrderProduct::remove_from_db(Session& s)
{
  Cache<Order>::remove(m_order_id);
  return DbEntity::remove(s);
}

bool OrderProduct::update_in_db(Session& s)
{
  std::tuple<size_t, size_t, int> values(m_product_id, m_order_id, m_quantity);
  Cache<Order>::remove(m_order_id);
  return DbEntity::update(s, g_fields, values);
}

std::vector<std::string> OrderProduct::fields() const
{
  return std::vector<std::string>(g_fields.begin(), g_fields.end());
}

std::vector<std::string> OrderProduct::values() const
{
  std::vector<std::string> v(g_fields.size());
  v[0] = std::to_string(id());
  v[1] = std::to_string(m_product_id);
  v[2] = std::to_string(m_order_id);
  v[3] = std::to_string(m_quantity);
  return v;
}

bool OrderProduct::fill_with(const std::vector<std::string>& fields, const std::vector<std::string>& values)
{
  bool ok = true;
  OrderProduct tmp(*this);
  for (int i = 0; i < (int)fields.size(); i++)
  {
    if (fields[i] == "id")
    {
      ok = read_id_callback(this, (char*)values[i].c_str());
      if (!ok) break;
    }
    else if (fields[i] == "productId")
    {
      ok = read_into(values[i], &m_product_id);
      if (!ok) break;
    }
    else if (fields[i] == "orderId")
    {
      ok = read_into(values[i], &m_order_id);
      if (!ok) break;
    }
    else if (fields[i] == "quantity")
    {
      ok = read_into(values[i], &m_quantity);
      if (!ok) break;
    }
  }
  if (!ok)
    *this = tmp;
  // remove from cache Order with old OrderProduct.id (new OrderProduct.id is removed in OrderProduct::update_in_db())
  Cache<Order>::remove(tmp.m_order_id);
  return ok;
}

static int read_callback(void* first, int argc, char** argv, char** cols)
{
  OrderProduct* op = static_cast<OrderProduct*>(first);
  std::vector<std::string> fields(argc), values(argc);
  for (int i = 0; i < argc; i++)
  {
    fields[i] = cols[i];
    values[i] = argv[i];
  }
  assert(op->fill_with(fields, values));
  return 0;
}