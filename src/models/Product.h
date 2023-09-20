#pragma once

#include "DbEntity.h"

class Product : public DbEntity
{
public:
  Product() : DbEntity("Product", Type::eProduct), m_supplier_id(-1), m_price(0), m_discount(0) {}
  size_t hash() const override;
  bool save_in_db(Session&) override;
  bool load_from_db(Session&, size_t) override;
  bool remove_from_db(Session&) override;
  bool update_in_db(Session&) override;
  std::vector<std::string> fields() const override;
  std::vector<std::string> values() const override;
  bool fill_with(const std::vector<std::string>& fields, const std::vector<std::string>& values) override;

  size_t supplier_id() const { return m_supplier_id; }
  float price() const { return m_price; }
  float price_with_discount() const { return m_price - (m_price * (m_discount / 100.f)); }
  float discount() const { return m_discount; }
  std::string& name() { return m_name; }
  const std::string& name() const { return m_name; }
  void set_supplier(size_t id) { m_supplier_id = id; }
  void set_name(const std::string& name) { m_name = name; }
  void set_price(float price) { m_price = price; }
  void set_discount(float discount) { m_discount = discount; }
private:
  size_t m_supplier_id;
  std::string m_name;
  float m_price;
  float m_discount;
};
