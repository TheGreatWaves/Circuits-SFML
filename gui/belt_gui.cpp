#include "belt_gui.hpp"
#include "board_gui.hpp"

void BeltGui::place_component()
{
	Context::instance()->board->add_component(std::move(m_active_component));
	m_active_component = nullptr;
}
