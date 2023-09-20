#pragma once

#include "imgui.h"
#include <string>
#include <vector>

constexpr int N_TABLES = 5;

namespace internal
{
  struct StateStorage
  {
    StateStorage();
    // instead of std::vector<bool>
    ImVector<bool> table_selection_states;
    ImVector<bool> popup_states;
    ImVector<bool> misc_states;
    std::string error_msg;
    // ImVector size is int
    int previously_selected_item;
    std::vector<std::string> input_text_buffers;

  };
};
