#pragma once

#include "DbEntity.h"

class OrderProduct : public DbEntity
{
public:
  OrderProduct() : DbEntity("OrderProduct", Type::eOrderProduct), m_product_id(-1), m_order_id(-1), m_quantity(0) {}
  size_t hash() const override;
  bool save_in_db(Session&) override;
  bool load_from_db(Session&, size_t) override;
  bool remove_from_db(Session&) override;
  bool update_in_db(Session&) override;
  std::vector<std::string> fields() const override;
  std::vector<std::string> values() const override;
  bool fill_with(const std::vector<std::string>& fields, const std::vector<std::string>& values) override;

  size_t product_id() const { return m_product_id; }
  size_t order_id() const { return m_order_id; }
  unsigned quantity() const { return m_quantity; }
  void set_product_id(size_t id) { m_product_id = id; }
  void set_order_id(size_t id) { m_order_id = id; }
  void set_quantity(int q) { m_quantity = q; }
private:
  size_t m_product_id;
  size_t m_order_id;
  unsigned m_quantity;
};
