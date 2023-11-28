#include "connection_port_gui.hpp"
#include "../gate.hpp"
#include "connection_gui.hpp"
#include "bus_gui.hpp"

void ConnectionPortGui::add_pin(const sf::Vector2f& pos, bool is_input)
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

void ConnectionPortGui::add_bus(const sf::Vector2f& pos, bool is_input, int bits)
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
    m_connections.emplace_back(BUS_HEIGHT, BUS_WIDTH, bits);
}

void ConnectionPortGui::add_connection(const sf::Vector2f& pos, bool is_input, Connection connection, int bits)
{
    switch (connection){
        break; case Connection::Pin:
        {
            add_pin(pos, is_input);
        }
        break; case Connection::Bus:
        {
            add_bus(pos, is_input, bits);
        }
    }
    m_connections.back().set_interactability(m_interactable);
    m_connections.back().set_position({m_strip.getPosition().x + (m_strip.getSize().x/2.f), pos.y});
}

// void ConnectionPortGui::add_pin(const sf::Vector2f& pos, bool is_input)
// {
// 	if (is_input)
//     {
//         Context::instance()->sketch->add_input_pin();
//     }
// 	else
// 	{
// 		Context::instance()->sketch->add_output_pin();
// 	}
//   m_connections.emplace_back(m_pin_rad);
//   m_connections.back().set_interactability(m_interactable);
//   m_connections.back().set_position({m_strip.getPosition().x + (m_strip.getSize().x/2.f), pos.y});
// }
