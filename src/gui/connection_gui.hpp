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

enum Connection {
    PIN = 0,
    BUS = 1,
};


class ConnectionGui
{
public:

    // Pin constructor
    ConnectionGui(float pin_radius = PIN_RADIUS)
    : connection_type(PIN)
    {
        m_pin = PinGui(pin_radius);
    }

    // Bus constructor
    ConnectionGui(float bus_height = BUS_HEIGHT, float bus_width = BUS_WIDTH, int m_bits = DEFAULT_BITS)
    : connection_type(BUS)
    {
        m_bus = BusGui(bus_height, bus_width, m_bits);
    }

    // Checks whether the position is within the connection
    bool contains(const sf::Vector2f& pos)
    {
        switch (connection_type)
        {
            break; case PIN:
            {
                return m_pin.contains(pos);
            }
            break; case BUS:
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
            break; case PIN:
            {
                return m_pin.handle_events(event);
            }
            break; case BUS:
            {
                return m_bus.handle_events(event);
            }
        }
    }

    // Draws the connection on the canvas
    void draw(sf::RenderTarget &target, sf::RenderStates states)
    {
        switch (connection_type)
        {
            break; case PIN:
            {
                return m_pin.draw(target, states);
            }
            break; case BUS:
            {
                return m_bus.draw(target, states);
            }
        }
    }
        
    // Sets the position of the connection
    void set_position(const sf::Vector2f& pos)
    {
        switch (connection_type)
        {
            break; case PIN:
            {
                return m_pin.set_position(pos);
            }
            break; case BUS:
            {
                return m_bus.set_position(pos);
            }
        }
    }

    // Returns the postiion of the connection
    [[ nodiscard ]] sf::Vector2f get_position() const
    {
        switch (connection_type)
        {
            break; case PIN:
            {
                return m_pin.get_position();
            }
            break; case BUS:
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
            break; case PIN:
            {
                return m_pin.getRadius();
            }
            break; case BUS:
            {
                return 0.f;
            }
        }
    }

    void set_interactability(bool interactable)
    {
        switch (connection_type)
        {
            break; case PIN:
            {
                return m_pin.set_interactability(interactable);
            }
            break; case BUS:
            {
                return m_bus.set_interactability(interactable);
            }
        }
    }

    bool is_on() const 
    {
        switch (connection_type)
        {
            break; case PIN:
            {
                return m_pin.is_on();
            }
            break; case BUS:
            {
                return m_bus.is_on();
            }
        }
    }

    void toggle_on() 
    {
        switch (connection_type)
        {
            break; case PIN:
            {
                return m_pin.toggle_on();
            }
            break; case BUS:
            {
                return m_bus.toggle_on();
            }
        }
    }

    void toggle_off() 
    {
        switch (connection_type)
        {
            break; case PIN:
            {
                return m_pin.toggle_off();
            }
            break; case BUS:
            {
                return m_bus.toggle_off();
            }
        }
    }

private:
    Connection connection_type = PIN;
    BusGui m_bus;
    PinGui m_pin;
};

#endif /* CONNECTION_GUI */