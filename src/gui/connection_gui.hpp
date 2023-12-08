#pragma once
#ifndef CONNECTION_GUI
#define CONNECTION_GUI

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics.hpp>

#include "../common.hpp"
#include <iostream>

const sf::Color ON_COLOR = sf::Color(220,20,60);
const sf::Color OFF_COLOR = sf::Color(88, 91, 112);
const float CONNECTION_WIDTH = 30.f;
constexpr float BUS_HEIGHT = 30.f;

class ConnectionGui 
{
    public:
        ConnectionGui(int m_bits = DEFAULT_CONNECTION_BITS)
        : m_bits { m_bits }
        , m_connection_height { BUS_HEIGHT + BUS_HEIGHT*m_bits/4 }
        , m_interactable { false }
        , box_touched { false }
        {
            m_connection = sf::RectangleShape(sf::Vector2(CONNECTION_WIDTH, m_connection_height));
            m_connection.setOutlineThickness(2);
            m_connection.setOutlineColor(sf::Color(88, 91, 112));
            for (int index = 0; index < m_bits; index++)
            {
                m_pins.emplace_back(false);
            }
            float m_pins_height = m_connection_height/m_bits-1;
            m_pins_shape = sf::RectangleShape(sf::Vector2(CONNECTION_WIDTH, m_pins_height-3));
            m_pin_y_pos_offset = m_pins_height + 1;
        }

        void set_position(const sf::Vector2f& pos)
        {
            m_connection.setPosition(pos);
        }

        void draw(sf::RenderTarget &target, sf::RenderStates states)
        {
            target.draw(m_connection, states);
            auto bus_pos = m_connection.getPosition();
            // Get a rectangle shape, draw it, shift it down, draw it again, until you get pins val.
            for (int index = 0; index < m_pins.size(); index++)
            {
                if (m_pins.at(index))
                {
                    m_pins_shape.setFillColor(ON_COLOR);
                }
                else
                {
                    m_pins_shape.setFillColor(OFF_COLOR);
                }
                m_pins_shape.setPosition({bus_pos.x - 20, bus_pos.y + 2 + ((index)*m_pin_y_pos_offset)});
                target.draw(m_pins_shape, states);
            }
        }

        bool contains_at_index(int index, const sf::Vector2f& pos)
        {
            if (index > m_pins.size())
            {
                return false;
            }
            auto bus_pos = m_connection.getPosition();
            m_pins_shape.setPosition({bus_pos.x - 20, bus_pos.y + 2 + ((index)*m_pin_y_pos_offset)});
            return  m_pins_shape.getGlobalBounds().contains(pos);
        }

        bool contains(const sf::Vector2f& pos)
        {
            for (int index = 0; index < m_pins.size(); index++)
            {
                if (contains_at_index(index, pos)) return true;
            }
            return false;
        }

        bool touches_box(const sf::Vector2f& pos)
        {
            return m_connection.getGlobalBounds().contains(pos);
        }

        void apply_bits(std::size_t bits)
        {
            std::size_t working_bits = bits;
            auto count = m_pins.size();
            std::size_t index = 0;
            while (count --> 0)
            {
                const bool condition = (working_bits >> count ) & 1;
                set_pin(index, condition);
                index++;
            }
        }

        void handle_events(const sf::Event& event)
        {
            if (!m_interactable) return;
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (touches_box(sf::Vector2f{static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)}))
                {
                    if (box_touched)
                    {
                        apply_bits(0);
                    }
                    else
                    {
                        apply_bits(Context::instance()->input_bus_bits);
                    }
                    box_touched = !box_touched;
                }
                else
                {
                    for (int index = 0; index < m_pins.size(); index++)
                    {
                        auto pressed = contains_at_index(index, sf::Vector2f{static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)});
                        if (pressed)
                        {
                            m_pins.at(index) = !m_pins.at(index);
                        }
                    }
                }
            }
        }

        int get_number_of_pins()
        {
            return m_pins.size();
        }

        std::vector<bool>* get_pins()
        {
            return &m_pins;
        }

        void set_pin(std::size_t index, bool value)
        {
            m_pins.at(index) = value;
        }

        void set_interactability(bool interactability)
        {
            m_interactable = interactability;
        }

        void add_pin()
        {
            m_pins.push_back(false);
        }

        float get_connection_height()
        {
            return m_connection_height;
        }

        sf::Vector2f get_position()
        {
            return m_connection.getPosition();
        }

        sf::Vector2f get_size()
        {
            return m_connection.getSize();
        }

    private:
        int m_bits;
        sf::RectangleShape m_connection;
        float m_connection_height;
        bool m_interactable;
        std::vector<bool> m_pins;
        sf::RectangleShape m_pins_shape;
        float m_pin_y_pos_offset;
        float m_pins_height;
        bool box_touched;
};

#endif /* CONNECTION_GUI */