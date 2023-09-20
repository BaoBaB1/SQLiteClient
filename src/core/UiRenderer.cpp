#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "UiRenderer.h"
#include "Internal.h"
#include "../utils/Utils.h"
#include "../models/Customer.h"
#include "../models/Supplier.h"
#include "../models/Product.h"
#include "../models/Order.h"
#include "../models/OrderProduct.h"
#include <algorithm>

// This method renders only given string values in table without any possibility to interact with table
void UiRenderer::render_table(const char* table, const vecstr& columns, const std::vector<vecstr>& values)
{
  const int ncolumns = (int)columns.size();
  const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
  const int visible_rows = std::min(8, (int)(values.size())) + 1;
  // When using ScrollX or ScrollY we need to specify a size for our table container!
  // Otherwise by default the table will fit all available space, like a BeginChild() call.
  ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * visible_rows);

  if (ImGui::BeginTable(table, ncolumns, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg, outer_size))
  {
    ImGui::TableNextRow();
    for (size_t i = 0; i < columns.size(); i++)
    {
      ImGui::TableSetColumnIndex((int)i);
      ImGui::Text(columns[i].c_str());
    }

    ImGuiListClipper clipper;
    clipper.Begin((int)values.size());

    while (clipper.Step())
    {
      for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
      {
        ImGui::TableNextRow();
        for (int column = 0; column < ncolumns; column++)
        {
          ImGui::TableSetColumnIndex(column);
          ImGui::Text(values[row][column].c_str());
        }
      }
    }
    ImGui::EndTable();
  }
}

// This method renders given entities in table with possibility to interact with table records (e.g edit, delete).
// All entities in vector must be the same type
void UiRenderer::render_table(const char* table, const std::vector<DbEntity*>& ents)
{
  if (ents.size() == 0)
    return; 
  const std::vector<std::string> columns = ents[0]->fields();
  std::vector<std::vector<std::string>> values(ents.size());
  for (size_t i = 0; i < ents.size(); i++)
    values[i] = std::move(ents[i]->values());

  const int ncolumns = (int)columns.size();
  const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
  const int visible_rows = std::min(8, (int)(values.size())) + 1;
  // When using ScrollX or ScrollY we need to specify a size for our table container!
  // Otherwise by default the table will fit all available space, like a BeginChild() call.
  ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * visible_rows);

  if (ImGui::BeginTable(table, ncolumns, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg, outer_size))
  {
    ImGui::TableNextRow();
    for (size_t i = 0; i < columns.size(); i++)
    {
      ImGui::TableSetColumnIndex((int)i);
      ImGui::Text(columns[i].data());
    }
    const int ss_index = entity_type_to_state_storage_index(ents[0]->type());
    internal::StateStorage& ss = m_state_storages[ss_index];
    ImVector<bool>& states = ss.table_selection_states;
    if (states.size() < values.size())
    {
      ImVector<bool> tmp;
      tmp.resize((int)values.size());
      std::copy(states.begin(), states.end(), tmp.begin());
      states = std::move(tmp);
    }

    ImGuiListClipper clipper;
    clipper.Begin((int)values.size());
    while (clipper.Step())
    {
      for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
      {
        ImGui::TableNextRow();
        bool is_row_selected = states[row];
        for (int column = 0; column < ncolumns; column++)
        {
          // must have to correctly handle selection
          ImGui::PushID(column);
          ImGui::TableSetColumnIndex(column);
          if (ImGui::Selectable(values[row][column].c_str(), &is_row_selected, ImGuiSelectableFlags_SpanAllColumns))
          {
            states[row] = !states[row];
            if (ss.previously_selected_item == -1)
            {
              ss.previously_selected_item = row;
            }
            else
            {
              states[ss.previously_selected_item] = false;
              if (ss.previously_selected_item == row)
                ss.previously_selected_item = -1;
              else 
                ss.previously_selected_item = row;
            }
          }
          ImGui::PopID();
        }

        ImVector<bool>& popup_states = ss.popup_states;
        bool& show_editing_window = popup_states[0];
        bool& show_delete_window = popup_states[1];
        bool& show_creation_window = popup_states[2];

        if (is_row_selected)
        {
          if (ImGui::BeginPopupContextItem())
          {
            if (ImGui::MenuItem("Create"))
              show_creation_window = true;
            if (ImGui::MenuItem("Edit"))
              show_editing_window = true;
            if (ImGui::MenuItem("Delete"))
              show_delete_window = true;
            ImGui::EndPopup();
          }
          if (show_creation_window)
            render_input_window("Entity addition",ents[row], &show_creation_window, false);
          if (show_editing_window)
            render_input_window("Entity editing", ents[row], &show_editing_window, true);
          if (show_delete_window)
          {
            std::string text = "Are you sure you want to delete selected " + utils::type_to_string(ents[row]->type()) + "?";
            render_informational_window("Deletion window", text, &show_delete_window, &Session::remove, ents[row]);
          }
        }
        
      }
    }
    ImGui::EndTable();
  }
}

void UiRenderer::render_input_window(const std::string& title, DbEntity* entity, bool* show_window, bool editing_mode)
{
  if (!entity)
    return;
  const int ss_index = entity_type_to_state_storage_index(entity->type());
  auto& state_storage = m_state_storages[ss_index];
  vecstr& input_text_buffers = state_storage.input_text_buffers;
  const vecstr columns = entity->fields();
  if (columns.size() > input_text_buffers.size())
    input_text_buffers.resize(columns.size());
  if (editing_mode)
  {
    if (input_text_buffers[0].empty())
      input_text_buffers = std::move(entity->values());
  }

  ImGui::OpenPopup(title.c_str());
  if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    const auto type = utils::type_to_string(entity->type());
    const float window_width = ImGui::GetWindowSize().x;
    const float text_width = ImGui::CalcTextSize(type.c_str()).x;
    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
    ImGui::Text(type.c_str());
    ImGui::Separator();
    ImGui::BeginDisabled();
    ImGui::InputText("Id", &input_text_buffers[0]);
    ImGui::EndDisabled();
    for (size_t i = 1; i < columns.size(); i++)
    {
      ImGui::Separator();
      ImGui::InputText(columns[i].data(), &input_text_buffers[i]);
    }

    bool finished = false;
    bool& error = state_storage.misc_states[0];
    std::string& error_desc = state_storage.error_msg;
    const float w = ImGui::GetWindowWidth();
    ImGui::SetCursorPosX(w / 2 - 60.f);

    if (ImGui::Button("Confirm"))
    {
      if (!entity->fill_with(columns, input_text_buffers))
      {
        error = true;
        error_desc = "Invalid input";
      }
      if (!error)
      {
        if (editing_mode)
        {
          if (!Session::instance().update(entity))
          {
            error = true;
            error_desc = "Could not update entity. Check foreign keys and input data.";
          }
        }
        else
        {
          if (!Session::instance().insert(entity))
          {
            error = true;
            error_desc = "Could not insert entity. Check foreign keys and input data.";
          }
        }
      }
      if (!error)
        finished = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
      finished = true;
    if (error)
      render_informational_window("Error", error_desc, &error);

    if (finished)
    {
      // clearing strings indicates that we have finished editing and on the next call these buffers will be filled with other data
      std::for_each(input_text_buffers.begin(), input_text_buffers.end(), [](std::string& s) { s.clear(); });
      error = false;
      *show_window = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::End();
  }
}

void UiRenderer::render_informational_window(const std::string& title, const std::string& text, bool* show_window, pSessionMemFunc ok_button_callback, DbEntity* entity)
{
  ImGui::OpenPopup(title.c_str());
  const auto size = ImVec2(250, 100);
  const auto pos = ImGui::GetWindowPos();
  const auto parent_size = ImGui::GetWindowSize();
  ImGui::SetNextWindowSize(size);
  ImGui::SetNextWindowPos(ImVec2(pos.x + (parent_size.x - size.x) / 2, pos.y + (parent_size.y - size.y) / 2));
  if (ImGui::BeginPopup(title.c_str()))
  {
    auto text_size = ImGui::CalcTextSize(text.c_str());
    // calc how many times text width larger than current window size and calc text height
    text_size.y = text_size.y * (text_size.x / size.x + 1);
    // min between current window width (+offset) and text width
    text_size.x = std::min(size.x - 30, text_size.x);
    const auto button_size = ImVec2(60, 20);
    const auto popup_pos = ImGui::GetWindowPos();
    ImGui::SetCursorPos(ImVec2((size.x - text_size.x) / 2, (size.y - text_size.y) / 2));
    ImGui::TextWrapped(text.c_str());
    ImGui::SetCursorPos(ImVec2(size.x / 2 - button_size.x, size.y - button_size.y - 10));
    bool finished = false;
    if (ImGui::Button("OK", button_size))
    {
      if (ok_button_callback)
        (Session::instance().*ok_button_callback)(entity);
      finished = true;
    }
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(size.x / 2 + 10, size.y - button_size.y - 10));
    if (ImGui::Button("Cancel", button_size))
      finished = true;
    if (finished)
    {
      *show_window = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void UiRenderer::render_menu_bar()
{
  if (ImGui::BeginMainMenuBar())
  {
    // bullshit but i'm tired of this project
    static bool visible = false;
    static DbEntity* p_entity = nullptr;
    if (ImGui::BeginMenu("Create"))
    {
      if (ImGui::MenuItem("Customer"))
      {
        visible = true;
        p_entity = new Customer;
      }
      if (ImGui::MenuItem("Order"))
      {
        visible = true;
        p_entity = new Order;
      }
      if (ImGui::MenuItem("Supplier"))
      {
        visible = true;
        p_entity = new Supplier;
      }
      if (ImGui::MenuItem("Product"))
      {
        visible = true;
        p_entity = new Product;
      }
      if (ImGui::MenuItem("OrderProduct"))
      {
        visible = true;
        p_entity = new OrderProduct;
      }
      ImGui::EndMenu();
    }
    if (visible)
    {
      render_input_window("Creation window", p_entity, &visible, false);
    }
    else
    {
      delete p_entity;
      p_entity = nullptr;
    }
    if (ImGui::BeginMenu("Tables"))
    {
      if (ImGui::MenuItem("Force update tables"))
      {
        Cache<Product>::clear();
        Cache<Supplier>::clear();
        Cache<OrderProduct>::clear();
        Cache<Customer>::clear();
        Cache<Order>::clear();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

int UiRenderer::entity_type_to_state_storage_index(DbEntity::Type type)
{
  switch (type)
  {
  case DbEntity::Type::eCustomer: 
    return 0;
  case DbEntity::Type::eOrder:
    return 1;
  case DbEntity::Type::eOrderProduct:
    return 2;
  case DbEntity::Type::eProduct:
    return 3;
  case DbEntity::Type::eSupplier:
    return 4; 
  }
  assert(false && "Never here");
  return -1;
}
