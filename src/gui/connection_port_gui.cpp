#include "connection_port_gui.hpp"
#include "../gate.hpp"
#include "connection_gui.hpp"


void ConnectionPortGui::add_pin(bool is_input)
{
    if (is_input)
    {
        Context::instance()->sketch->add_input_pin();
    }
	else
	{
		Context::instance()->sketch->add_output_pin();
	}
    m_connections.emplace_back(m_pin_rad);
}

void ConnectionPortGui::add_bus(bool is_input, int bits)
{
    /* TODO: Essentially store bit_0 - bit_n as normal pins and then keep track of the values
    where the bits start and end to define the bus */ 
    if (is_input)
    {
        Context::instance()->sketch->add_input_bus(bits);
    }
	else
	{
		Context::instance()->sketch->add_output_bus(bits);
	}
    m_connections.emplace_back(m_pin_rad);
}

void ConnectionPortGui::add_connection(const sf::Vector2f& pos, bool is_input, Connection connection=Connection::PIN, int bits = 0)
{
    switch (connection){
        break; case Connection::PIN:
        {
            add_pin(is_input);
        }
        break; case Connection::BUS:
        {
            add_bus(is_input, bits);
        }
    }
    m_connections.back().set_interactability(m_interactable);
    m_connections.back().set_position({m_strip.getPosition().x + (m_strip.getSize().x/2.f), pos.y});
}
