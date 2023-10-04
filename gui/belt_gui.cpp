#include "belt_gui.hpp"
#include "board_gui.hpp"

void BeltGui::place_component()
{
	Context::instance()->board->add_component(std::move(m_active_component));
	m_active_component = nullptr;
}

void BeltGui::select_belt()
{
  switch (m_active_index)
  {
    break; case -2:
    {
      const auto& name = Context::instance()->current_component_name;

      if (!name.empty())
      {
        create_new_component();
      }
    }
    break; case -1: return;
    break; default:
    {
      auto names = Board::instance()->get_names();
      auto chosen = names[m_active_index];
      m_active_component = std::make_unique<ComponentGui>(chosen);
    }
  }
}

void BeltGui::create_new_component()
{
  Context::instance()->board->save_current_configuration();
}
