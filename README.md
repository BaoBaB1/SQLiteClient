# SQLiteClient
This project is a sample of working with SQLite database using C++. It supports CRUD operations over database and has GUI.
# Database schema
<p align="center">
  <img src=images/db_logical_schema.png?raw=true/>
</p>

# Screenshots
<p align="center">
  <img src=images/demo_1.png?raw=true "Main window">
</p>
<p align="center">
  <img src=images/demo_2.png?raw=true>
</p>

# If you want run it locally
1) Download sqlite3
2) Create your sqlite database inside project folder with provided schema (sqlite3 your_db_name.db < db_schema.txt)
3) Generate solution from CMakeLists.txt using CMake
4) In Session.cpp change "ShopManagementSystem.db" on your database name
5) [optional] In main.cpp uncomment function fill_tables() if you want to fill all tables with some records
