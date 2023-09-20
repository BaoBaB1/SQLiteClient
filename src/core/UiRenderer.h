#pragma once

#include <vector>
#include <string>
#include "Internal.h"
#include "Session.h"
#include "../models/DbEntity.h"

class UiRenderer
{
  using vecstr = std::vector<std::string>;
  using pSessionMemFunc = bool(Session::*)(DbEntity*);
public:
  void render_menu_bar();
  template<typename Entity>
  void render_table(const char* table);
  void render_table(const char* table, const vecstr& columns, const std::vector<vecstr>& values);
  void render_table(const char* table, const std::vector<DbEntity*>& ents);
  void render_input_window(const std::string& title, DbEntity* entity, bool* show_window, bool editing_mode);
  void render_informational_window(const std::string& title, const std::string& text, bool* show_window, pSessionMemFunc ok_button_callback = nullptr, DbEntity* entity = nullptr);
private:
  int entity_type_to_state_storage_index(DbEntity::Type type);
private:
  std::array<internal::StateStorage, N_TABLES> m_state_storages;
};

template<typename Entity>
void UiRenderer::render_table(const char* table)
{
  auto& session = Session::instance();
  auto records = session.all<Entity>();
  std::vector<DbEntity*> ents(records.size());
  for (size_t i = 0; i < records.size(); i++)
    ents[i] = &records[i];
  render_table(table, ents);
}
