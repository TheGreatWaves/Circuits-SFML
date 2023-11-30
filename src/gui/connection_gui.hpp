#pragma once
#ifndef CONNECTION_GUI
#define CONNECTION_GUI

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <iostream>
#include <vector>
#include "bus_gui.hpp"
#include "pin_gui.hpp"

enum class Connection {
    Pin,
    Bus,
    BusMember, // Essentially a pin
};


class ConnectionGui
{
public:
    // Connection constructor
    ConnectionGui(){}

    // Pin constructor
    ConnectionGui(float pin_radius = PIN_RADIUS, bool bus_member = false)
    {
        std::cout << "Pin constructor called" << std::endl;
        m_pin = PinGui(pin_radius);
        if (bus_member)
        {
            m_connection_type = Connection::BusMember;
        }
        std::cout << "Pin created" << std::endl;
    }

    // Bus constructor
    ConnectionGui(int front_index, int m_bits = DEFAULT_BITS, float bus_height = BUS_HEIGHT, float bus_width = BUS_WIDTH, Connection connection_type = Connection::Pin)
    {
        if (connection_type != Connection::Bus){
            return;
        }
        m_connection_type = Connection::Bus;
        std::cout << "Bus constructor called" << std::endl;
        m_bus = BusGui(front_index, m_bits, bus_height, bus_width);
        std::cout << "Bus constructor created" << std::endl;
    }

    // Checks whether the position is within the connection
    bool contains(const sf::Vector2f& pos)
    {
        switch (m_connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin->contains(pos);
            }
            break; case Connection::Bus:
            {
                return m_bus->contains(pos);
            }
        }
    }

    // Handles inputs/interactions with the connections
    void handle_events(const sf::Event& event)
    {
        switch (m_connection_type)
        {
            break; case Connection::Pin:
            {
                return m_pin->handle_events(event);
            }
            break; case Connection::Bus:
            {
                return m_bus->handle_events(event);
            }
            break; default:
            {}
        }
    }

    // Draws the connection on the canvas
    void draw(sf::RenderTarget &target, sf::RenderStates states)
    {
        switch (m_connection_type)
        {
            break; case Connection::Pin:
            {
                std::cout << "Pin drawing" << std::endl;
                return m_pin->draw(target, states);
                std::cout << "Pin drawn" << std::endl;
            }
            break; case Connection::Bus:
            {
                std::cout << "Bus drawing" << std::endl;
                return m_bus->draw(target, states);
                std::cout << "Bus drawn" << std::endl;
            } 
            break; default:
            {}
        }
    }
        
    // Sets the position of the connection
    void set_position(const sf::Vector2f& pos)
    {
        switch (m_connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin->set_position(pos);
            }
            break; case Connection::Bus:
            {
                return m_bus->set_position(pos);
            }
            break; default:
            {}
        }
    }

    // Returns the width of the bus
    float get_bus_width()
    {
        return m_bus->get_width();
    }

    void apply_bus_bits(){
        // TODO: Go to bus connection and then apply on the busmembers
    }

    // Returns the postiion of the connection
    [[ nodiscard ]] sf::Vector2f get_position() const
    {
        switch (m_connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin->get_position();
            }
            break; case Connection::Bus:
            {
                return m_bus->get_position();
            }
        }
    }

    // For pins this will return the radius of the pin
    float get_radius() const
    {
        switch (m_connection_type)
        {
            break; case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin->get_radius();
            }
            break; case Connection::Bus:
            {
                return 0.f;
            }
        }
    }

    void set_interactability(bool interactable)
    {
        switch (m_connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin->set_interactability(interactable);
            }
            break; case Connection::Bus:
            {
                return m_bus->set_interactability(interactable);
            }
            break; default:
            {}
        }
    }

    bool is_on() const 
    {
        switch (m_connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin->is_on();
            }
            break; case Connection::Bus:
            {
                return m_bus->is_on();
            }
            break; default:
            {
                return false;
            }
        }
    }

    void toggle_on() 
    {
        switch (m_connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin->toggle_on();
            }
            break; case Connection::Bus:
            {
                return m_bus->toggle_on();
            }
            break; default:
            {}
        }
    }

    void toggle_off() 
    {
        switch (m_connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin->toggle_off();
            }
            break; case Connection::Bus:
            {
                return m_bus->toggle_off();
            }
            break; default:
            {}
        }
    }

    void set_connection_type(Connection connection)
    {
        m_connection_type = connection;
    }

private:
    Connection m_connection_type = Connection::Pin;
    std::optional<BusGui> m_bus;
    std::optional<PinGui> m_pin;
};

#endif /* CONNECTION_GUI */