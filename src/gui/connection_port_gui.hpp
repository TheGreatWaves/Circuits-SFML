#pragma once
#ifndef CONNECTION_PORT_GUI
#define CONNECTION_PORT_GUI

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <iostream>
#include <vector>
#include "connection_gui.hpp"
#include "context.hpp"

class ConnectionPortGui
{
public:
    ConnectionPortGui(float pin_radius)
    : m_pin_rad(pin_radius)
    {
        m_strip.setFillColor(sf::Color::Transparent);
    }

    void set_interactability(bool interactable)
    {
        m_interactable = interactable;
    }

    std::pair<std::size_t, ConnectionGui*> get_pin(const sf::Vector2f& pos)
    {
        std::size_t index = 0;
        for (auto& p : m_connections)
        {
            // Ugly and wasteful but it has to be done.
            if (p.contains(pos))
            {
                return {index, &p};
            }
            index++;
        }
        return {0, nullptr};
    }

    std::size_t get_bits()
    {
        std::size_t bits = 0;
        for (auto& pin : m_connections)
        {
        bits <<= 1;
        bits |= (pin.is_on() ? 1 : 0);
        }
        return bits;
    }

    void clear_port()
    {
        m_connections.clear();
    }

    void apply_bits(std::size_t bits)
    {
        auto count = m_connections.size();
        std::size_t index = 0;
        while (count --> 0)
        {
            if ((bits >> count ) & 1) 
            {
                m_connections[index].toggle_on();
            }
            else
            {
                m_connections[index].toggle_off();
            }
            index++;
        }
    }

    void info()
    {
        std::cout << "=== Pins ===\n";
        for (std::size_t i = 0; i < m_connections.size(); i++)
        {
        std::cout << "Pin[" << i << "]: " << (m_connections[i].is_on() ? 1 : 0) << '\n';
        }
        std::cout << '\n';
    }

    std::size_t size()
    {
      return m_connections.size();
    }

    void handle_events(const sf::Event& event, bool is_input = true, int bits = 1)
	{
        if (Context::instance()->edit_mode == Mode::WIRING)
        {
            return;
        }

        bool pressed_connection = false;

        sf::Vector2f mouse_pos = {static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)};

        for (auto& p : m_connections)
        {
            p.handle_events(event);
            // Ugly and wasteful but it has to be done.
            if (p.contains(mouse_pos))
            {
                pressed_connection = true;
            }
        }

        if (!pressed_connection && event.type == sf::Event::MouseButtonPressed)
        {
            auto pressed = (m_strip.getGlobalBounds().contains(mouse_pos));

        if (pressed)
        {
            std::cout << "Pressed To Add Connection\n";
            if (Context::instance()->edit_mode == Mode::IDLE)
            {
                add_connection(mouse_pos, is_input, Connection::Pin, bits);
            }
            else 
            {
                std::cout << "Adding bus at handle_events \n";
                add_connection(mouse_pos, is_input, Connection::Bus, bits);
            }
        }
        }
	}

    void add_connection(const sf::Vector2f& pos, bool is_input, Connection connection, int bits);

    void add_pin(const sf::Vector2f& pos, bool is_input, bool is_bus_member);

    void add_bus(const sf::Vector2f& pos, bool is_input, int bits = 1);

    void draw(sf::RenderTarget &target, sf::RenderStates states) 
    {
        target.draw(m_strip, states);
        for (auto& p : m_connections)
        {
            p.draw(target, states);
        }   
    }

    void set_position(const sf::Vector2f& pos)
    {
        m_strip.setPosition(pos);
    }

    sf::Vector2f get_size() const
    {
        return m_strip.getSize();
    }

    void set_size(const sf::RectangleShape& parent)
    {
        m_strip.setSize({10.f, parent.getSize().y});
    }

    void setup_port(std::size_t size)
    {
        // Start from a blank slate.
        m_connections.clear();

        auto segments = static_cast<float>(size + 1);
        auto segment_size = m_strip.getSize().y / segments;

        for (std::size_t i = 0; i < size; i++)
        {
            m_connections.emplace_back(m_pin_rad);
            auto& pin = m_connections.back();
            auto strip_pos = m_strip.getPosition();
            auto strip_size = m_strip.getSize();
            pin.set_position({strip_pos.x + (strip_size.x / 2.f), strip_pos.y + ((i + 1) * segment_size)});
        }
    }

    void anchor(const sf::RectangleShape& base, bool lhs = true)
    {
        this->set_size(base);
        auto base_gbounds = base.getGlobalBounds();

        if (lhs)
        {
        this->set_position({base_gbounds.left-(this->get_size().x / 2.f), base_gbounds.top});
        }
        else
        {
        this->set_position({base_gbounds.left + base_gbounds.width - (this->get_size().x / 2.f), base_gbounds.top});
        }

        auto size = m_connections.size();
        auto segments = static_cast<float>(size + 1);
        auto segment_size = m_strip.getSize().y / segments;

        for (std::size_t i = 0; i < size; i++)
        {
        auto& pin = m_connections[i];
        auto strip_pos = m_strip.getPosition();
        auto strip_size = m_strip.getSize();
        pin.set_position({strip_pos.x + (strip_size.x / 2.f), strip_pos.y + ((i + 1) * segment_size)});
        }
    }



private:
    bool m_interactable = false;
    float m_pin_rad;
    sf::RectangleShape  m_strip;
    std::vector<ConnectionGui> m_connections;
};

#endif /* CONNECTION_PORT_GUI */