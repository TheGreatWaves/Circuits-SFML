#include "pin_port_gui.hpp"
#include "../digital_serializer.hpp"

void PinPortGui::add_pin(const sf::Vector2f& pos, bool is_input)
{
	if (is_input)
  {
    Context::instance()->sketch->add_input_pin();
  }
	else
	{
		Context::instance()->sketch->add_output_pin();
	}

  m_pins.emplace_back(m_pin_rad);
  m_pins.back().set_interactability(m_interactable);
  m_pins.back().set_position({m_strip.getPosition().x + (m_strip.getSize().x/2.f), pos.y});
}
