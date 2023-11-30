#include "connection_port_gui.hpp"
#include "../gate.hpp"
#include "connection_gui.hpp"
#include "bus_gui.hpp"

void ConnectionPortGui::add_pin(const sf::Vector2f& pos, bool is_input, bool is_bus_member)
{
    if (is_input)
    {
        Context::instance()->sketch->add_input_pin();
    }
	else
	{
		Context::instance()->sketch->add_output_pin();
	}
    std::cout << "Adding pin in connection_port_gui.cpp" << std::endl;
    m_connections.emplace_back(m_pin_rad, is_bus_member);
    m_connections.back().set_interactability(m_interactable);
    m_connections.back().set_position({m_strip.getPosition().x + (m_strip.getSize().x/2.f), pos.y});
}

void ConnectionPortGui::add_bus(const sf::Vector2f& pos, bool is_input, int bits)
{
    /* TODO: Essentially store bit_0 - bit_n as normal pins and then keep track of the values
    where the bits start and end to define the bus */ 
    m_connections.emplace_back(m_connections.size(), bits, BUS_HEIGHT, BUS_WIDTH, Connection::Bus);
    m_connections.back().set_interactability(m_interactable);
    m_connections.back().set_position({m_strip.getPosition().x + (m_connections.back().get_bus_width()/2.f) + (m_strip.getSize().x/2.f), pos.y});
    bits += -1;
    std::cout << "Added bus to m_connections" << std::endl;
    while (bits --> 0)
    {
        add_pin(pos, is_input, true);
    }
}

void ConnectionPortGui::add_connection(const sf::Vector2f& pos, bool is_input, Connection connection, int bits)
{
    switch (connection){
        break; case Connection::Pin:
        {
            add_pin(pos, is_input, false);
        }
        break; case Connection::Bus:
        {
            std::cout << "Adding bus in add_connection" << std::endl;
            add_bus(pos, is_input, bits);
        }
        break; default: 
        {}
    }
}