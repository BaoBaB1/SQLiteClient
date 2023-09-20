#include "Internal.h"

internal::StateStorage::StateStorage()
{
  popup_states.resize(5, false);
  table_selection_states.resize(5, false);
  misc_states.resize(5, false);
  previously_selected_item = -1;
}
