#pragma once

#include "DbEntity.h"

class Order : public DbEntity
{
public:
  Order() : DbEntity("'Order'", Type::eOrder), m_total_price(0), m_customer_id(-1) {}
  size_t hash() const override;
  bool save_in_db(Session&) override;
  bool load_from_db(Session&, size_t) override;
  bool remove_from_db(Session&) override;
  bool update_in_db(Session&) override;
  std::vector<std::string> fields() const override;
  std::vector<std::string> values() const override;
  bool fill_with(const std::vector<std::string>& fields, const std::vector<std::string>& values) override;

  size_t customer_id() const { return m_customer_id; }
  float total_price() const { return m_total_price; }
  std::string& delivery_address() { return m_delivery_address; }
  std::string& status() { return m_status; }
  const std::string& delivery_address() const { return m_delivery_address; }
  const std::string& status() const { return m_status; }
  void set_customer_id(size_t id) { m_customer_id = id; }
  void set_total_price(float price) { m_total_price = price; }
  void set_delivery_address(const std::string& adr) { m_delivery_address = adr; }
  void set_status(const std::string& status) { m_status = status; }

private:
  float m_total_price;
  std::string m_delivery_address;
  std::string m_status;
  size_t m_customer_id;
};
