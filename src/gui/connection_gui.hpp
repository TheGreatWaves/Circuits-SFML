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

    // Pin constructor
    ConnectionGui(float pin_radius = PIN_RADIUS, bool bus_member = false)
    {
        m_pin = PinGui(pin_radius);
        if (bus_member)
        {
            connection_type = Connection::BusMember;
        }
    }

    // Bus constructor
    ConnectionGui(int front_index, int m_bits = DEFAULT_BITS, float bus_height = BUS_HEIGHT, float bus_width = BUS_WIDTH)
    : connection_type(Connection::Bus)
    {
        m_bus = BusGui(front_index, m_bits, bus_height, bus_width);
    }

    // Checks whether the position is within the connection
    bool contains(const sf::Vector2f& pos)
    {
        switch (connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin.contains(pos);
            }
            break; case Connection::Bus:
            {
                return m_bus.contains(pos);
            }
        }
    }

    // Handles inputs/interactions with the connections
    void handle_events(const sf::Event& event)
    {
        switch (connection_type)
        {
            break; case Connection::Bus:
            {
                return m_pin.handle_events(event);
            }
            break; case Connection::Pin:
            {
                return m_bus.handle_events(event);
            }
            break; default:
            {}
        }
    }

    // Draws the connection on the canvas
    void draw(sf::RenderTarget &target, sf::RenderStates states)
    {
        switch (connection_type)
        {
            break; case Connection::Pin:
            {
                return m_pin.draw(target, states);
            }
            break; case Connection::Bus:
            {
                std::cout << "Calling bus draw\n";
                return m_bus.draw(target, states);
                std::cout << "Done calling bus draw\n";
            }
            break; default:
            {}
        }
    }
        
    // Sets the position of the connection
    void set_position(const sf::Vector2f& pos)
    {
        switch (connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin.set_position(pos);
            }
            break; case Connection::Bus:
            {
                return m_bus.set_position(pos);
            }
            break; default:
            {}
        }
    }

    // Returns the postiion of the connection
    [[ nodiscard ]] sf::Vector2f get_position() const
    {
        switch (connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin.get_position();
            }
            break; case Connection::Bus:
            {
                return m_bus.get_position();
            }
        }
    }

    // For pins this will return the radius of the pin
    float get_radius() const
    {
        switch (connection_type)
        {
            break; case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin.get_radius();
            }
            break; case Connection::Bus:
            {
                return 0.f;
            }
        }
    }

    void set_interactability(bool interactable)
    {
        switch (connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin.set_interactability(interactable);
            }
            break; case Connection::Bus:
            {
                return m_bus.set_interactability(interactable);
            }
            break; default:
            {}
        }
    }

    bool is_on() const 
    {
        switch (connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin.is_on();
            }
            break; case Connection::Bus:
            {
                return m_bus.is_on();
            }
            break; default:
            {
                return false;
            }
        }
    }

    void toggle_on() 
    {
        switch (connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin.toggle_on();
            }
            break; case Connection::Bus:
            {
                return m_bus.toggle_on();
            }
            break; default:
            {}
        }
    }

    void toggle_off() 
    {
        switch (connection_type)
        {
            break; 
            case Connection::Pin:
            case Connection::BusMember:
            {
                return m_pin.toggle_off();
            }
            break; case Connection::Bus:
            {
                return m_bus.toggle_off();
            }
            break; default:
            {}
        }
    }

private:
    Connection connection_type = Connection::Pin;
    BusGui m_bus;
    PinGui m_pin;
};

#endif /* CONNECTION_GUI */