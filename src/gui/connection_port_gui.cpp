#include "connection_port_gui.hpp"
#include "../gate.hpp"

void ConnectionPortGui::add_connection(const sf::Vector2f& pos, bool is_input, int bits)
{
	if (is_input)
    {
        for (int index = 0; index < bits; index++)
        {
            Context::instance()->sketch->add_input_pin();
        }
    }
	else
	{
		for (int index = 0; index < bits; index++)
        {
            Context::instance()->sketch->add_output_pin();
        }
	}

  m_connections.emplace_back(bits);
  m_connections.back().set_interactability(m_interactable);
  m_connections.back().set_position({m_strip.getPosition().x + (m_strip.getSize().x/2.f), pos.y});
}