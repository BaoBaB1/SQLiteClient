#pragma once

#include "DbEntity.h"

class Customer : public DbEntity
{
public:
  Customer() : DbEntity("Customer", Type::eCustomer), m_age(0) {}
  size_t hash() const override;
  bool save_in_db(Session&) override;
  bool load_from_db(Session&, size_t) override;
  bool remove_from_db(Session&) override;
  bool update_in_db(Session&) override;
  std::vector<std::string> fields() const override;
  std::vector<std::string> values() const override;
  bool fill_with(const std::vector<std::string>& fields, const std::vector<std::string>& values) override;

  int age() const { return m_age; }
  std::string& name() { return m_name; }
  std::string& phone() { return m_phone; }
  std::string& country() { return m_country; }
  std::string& city() { return m_city; }
  std::string& address() { return m_address; }
  const std::string& name() const { return m_name; }
  const std::string& phone() const { return m_phone; }
  const std::string& country() const { return m_country; }
  const std::string& city() const { return m_city; }
  const std::string& address() const { return m_address; }
  void set_age(unsigned age) { m_age = age; }
  void set_name(const std::string& name) { m_name = name; }
  void set_phone(const std::string& phone) { m_phone = phone; }
  void set_country(const std::string& country) { m_country = country; }
  void set_city(const std::string& city) { m_city = city; }
  void set_address(const std::string& address) { m_address = address; }

private:
  unsigned m_age;
  std::string m_name;
  std::string m_phone;
  std::string m_country;
  std::string m_city;
  std::string m_address;
};
