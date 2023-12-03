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

class ConnectionGui;

struct Connection_Val {
    BusGui* m_bus;
    PinGui* m_pin;
    ConnectionGui* m_connection;
};

class ConnectionGui
{
public:
    // Connection constructor
    ConnectionGui(){}

    // Pin constructor
    ConnectionGui(float pin_radius = PIN_RADIUS, bool bus_member = false)
    : m_pin{std::make_unique<PinGui>(pin_radius)}
    {
        std::cout << "Pin constructor called\n";
        if (bus_member)
        {
            m_connection_type = Connection::BusMember;
        }
        std::cout << "Pin created\n";
    }

    // Bus constructor
    ConnectionGui(int front_index, int m_bits = DEFAULT_BITS, float bus_height = BUS_HEIGHT, float bus_width = BUS_WIDTH, Connection connection_type = Connection::Pin)
    : m_bus{std::make_unique<BusGui>(front_index, m_bits, bus_height, bus_width)}  
    {
        if (connection_type != Connection::Bus){
            return;
        }
        m_connection_type = Connection::Bus;
        std::cout << "Bus constructor called\n";
        std::cout << "Bus created in constructor: " << m_bus << "\n";
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
                std::cout << "Pin drawing: " << this << '\n';
                return m_pin->draw(target, states);
                // std::cout << "Pin drawn" << std::endl;
            }
            break; case Connection::Bus:
            {
                std::cout << "Bus drawing: " << this << '\n';
                return m_bus->draw(target, states);
                // std::cout << "Bus drawn" << std::endl;
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

    std::vector<PinGui*> get_bus_members()
    {
        if (m_connection_type == Connection::Bus)
        {
            return m_bus->get_members();
        }
        else
        {
            return {};
        }
    }

    // Returns the width of the bus
    float get_bus_width()
    {
        if (m_connection_type == Connection::Bus)
        {
            return m_bus->get_width();
        }
        else
        {
            return 0.f;
        }
    }

    // Returns the height of the bus
    float get_bus_height()
    {
        if (m_connection_type == Connection::Bus)
        {
            return m_bus->get_height();
        }
        else
        {
            return 0.f;
        }
    }

    // Adds a pin to the bus as its member
    void add_bus_member(PinGui* pin)
    {
        if (m_connection_type == Connection::Bus)
        {
            m_bus->add_member_pin(pin);
        }
        return;
    }

    // Returns the pin of a connection
    // PinGui* get_pin()
    std::vector<Connection_Val> get_pins()
    {
        std::vector<Connection_Val> output = {};
        if (m_connection_type != Connection::Bus)
        {
            Connection_Val output_pin = { nullptr, m_pin.get(), this };
            output.push_back(output_pin);
            return output;
        }
        else
        {
            output.push_back({ m_bus.get() , nullptr });
            auto bus_members = m_bus->get_members();
            for (int index = 0; index < bus_members.size(); index++)
            {
                output.push_back({ nullptr, bus_members.at(index) });
            }
            return output;
        }
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
                if (m_pin != nullptr)
                {
                    return m_pin->set_interactability(interactable);
                }
            }
            break; case Connection::Bus:
            {
                if (m_bus != nullptr)
                {
                    return m_bus->set_interactability(interactable);
                }
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

    int get_bus_member_size()
    {
        return m_bus->get_member_size();
    }
    
    BusGui* get_bus()
    {
        return m_bus.get();
    }
    
    PinGui* get_pin()
    {
        return m_pin.get();
    }

    Connection m_connection_type = Connection::Pin;
private:
    std::unique_ptr<BusGui> m_bus;
    std::unique_ptr<PinGui> m_pin;
};

#endif /* CONNECTION_GUI */