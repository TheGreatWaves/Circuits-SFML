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

class ConnectionPortGui
{
public:
    ConnectionPortGui(){
        m_strip.setFillColor(sf::Color::Transparent);
    }

    void set_interactability(bool interactable)
    {
        m_interactable = interactable;
    }

    void handle_events(const sf::Event& event, bool is_input = true)
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
            add_connection(mouse_pos, is_input);
        }
        }
	}

    void add_connection(const sf::Vector2f& pos, bool is_input, Connection connection = PIN, int bits = 0);

    void add_pin(bool is_input);

    void add_bus(bool is_input, int bits = 0);

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


private:
    bool m_interactable = false;
    float m_pin_rad = PIN_RADIUS;
    sf::RectangleShape  m_strip;
    std::vector<ConnectionGui> m_connections;
};

#endif /* CONNECTION_PORT_GUI */