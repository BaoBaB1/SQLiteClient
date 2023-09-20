#include <stdexcept>
#include "core/Session.h"
#include "models/Customer.h"
#include "models/Order.h"
#include "models/Supplier.h"
#include "models/Product.h"
#include "models/OrderProduct.h"
#include "core/MainWindow.h"

void fill_tables();

int main()
{
  try
  {
    //fill_tables();
    MainWindow& window = MainWindow::instance();
    window.show();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what();
  }
  return 0;
}

void fill_tables()
{
  Session& session = Session::instance();
  Customer c, c2;
  c.set_address("221B Baker Street");
  c.set_age(41);
  c.set_city("London");
  c.set_country("UK");
  c.set_name("Sherlock Holmes");
  c.set_phone("+447975777666");

  c2.set_address("Chreshatyk 21");
  c2.set_age(26);
  c2.set_city("Kyiv");
  c2.set_country("Ukraine");
  c2.set_name("Maria DDD");
  c2.set_phone("0961111112");
  session.insert(&c);
  session.insert(&c2);

  Supplier s, s2, s3;
  s.set_address("Bucok street 31C");
  s.set_city("Cupertino");
  s.set_country("USA");
  s.set_name("Apple");
  s.set_phone("14842989687");

  s2.set_address("Pershing street");
  s2.set_city("Krakow");
  s2.set_country("Poland");
  s2.set_name("Wirki dzinki");
  s2.set_phone("382940101123");

  s3.set_address("Avenue des Champs-Elysees");
  s3.set_city("Paris");
  s3.set_country("France");
  s3.set_name("Chanel");
  s3.set_phone("94321949124");
  session.insert(&s);
  session.insert(&s2);
  session.insert(&s3);

  Product p, p2, p3, p4;
  p.set_supplier(s.id());
  p.set_name("Iphone 13 Pro Max 256 gb");
  p.set_price(1100);

  p2.set_supplier(s.id());
  p2.set_name("Airpods Pro II");
  p2.set_price(300);
  p2.set_discount(5);

  p3.set_supplier(s2.id());
  p3.set_name("HyperX Cloud II");
  p3.set_price(200);

  p4.set_supplier(s3.id());
  p4.set_name("Man perfumes EQS");
  p4.set_price(80);

  session.insert(&p);
  session.insert(&p2);
  session.insert(&p3);
  session.insert(&p4);

  Order o;
  OrderProduct op, op2;
  op.set_product_id(p.id());
  op.set_quantity(1);
  op2.set_product_id(p2.id());
  op2.set_quantity(2);
  o.set_customer_id(c.id());
  o.set_delivery_address(c.country() + ", " + c.city() + " " + c.address());
  o.set_status("In progress");
  session.insert(&o);
  op.set_order_id(o.id());
  op2.set_order_id(o.id());
  session.insert(&op);
  session.insert(&op2);

  Order o2;
  OrderProduct op3;
  op3.set_product_id(p3.id());
  op3.set_quantity(1);
  o2.set_customer_id(c2.id());
  o2.set_delivery_address(c2.country() + ", " + c2.city() + " " + c2.address());
  o2.set_status("Finished");
  session.insert(&o2);
  op3.set_order_id(o2.id());
  session.insert(&op3);
}
