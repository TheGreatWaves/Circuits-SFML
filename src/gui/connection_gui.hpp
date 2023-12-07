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
        , m_connection_height { m_bits*BUS_HEIGHT/4 }
        , m_pins_shape { sf::RectangleShape(sf::Vector2(CONNECTION_WIDTH, m_connection_height/m_bits-2)) }
        , m_interactable { false }
        {
            m_bus = sf::RectangleShape(sf::Vector2(CONNECTION_WIDTH, m_connection_height));
            for (int index = 0; index < m_bits; index++)
            {
                m_pins.emplace_back(false);
            }
        }

        void set_position(const sf::Vector2f& pos)
        {
            m_bus.setPosition(pos);
        }

        void draw(sf::RenderTarget &target, sf::RenderStates states)
        {
            target.draw(m_bus, states);
            auto bus_pos = m_bus.getPosition();
            // Get a rectangle shape, draw it, shift it down, draw it again, until you get pins val.
            for (int index = 0; index < m_pins.size(); index++)
            {
                // std::cout << "draw1\n";
                // std::cout << "draw m_pins at index: " << index << " value: " << m_pins.at(index) << "\n";
                if (m_pins.at(index))
                {
                    m_pins_shape.setFillColor(ON_COLOR);
                }
                else
                {
                    m_pins_shape.setFillColor(OFF_COLOR);
                }
                // std::cout << "draw2\n";
                m_pins_shape.setPosition({bus_pos.x - 20, bus_pos.y + ((index)*m_connection_height/m_bits-2+2)});
                target.draw(m_pins_shape, states);
            }
        }

        bool contains_at_index(int index, const sf::Vector2f& pos)
        {
            if (index > m_pins.size())
            {
                return false;
            }
            auto bus_pos = m_bus.getPosition();
            m_pins_shape.setPosition({bus_pos.x - 20, bus_pos.y + ((index)*m_connection_height/m_bits-2+2)});
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
            return m_bus.getGlobalBounds().contains(pos);
        }

        void handle_events(const sf::Event& event)
        {
            if (!m_interactable) return;

            if (event.type == sf::Event::MouseButtonPressed)
            {
                for (int index = 0; index < m_pins.size(); index++)
                {
                    auto pressed = contains_at_index(index, sf::Vector2f{static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)});
                    if (pressed)
                    {
                        std::cout << "handle-events1\n";
                        m_pins.at(index) = !m_pins.at(index);
                        std::cout << "handle-events2\n";
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
            // if (value) std::cout << "Set to true\n";
            // if (!value) std::cout << "Set to false\n";
            std::cout << "Old val: " << m_pins.at(index) << "\n";
            m_pins.at(index) = value;
            for (int index = 0; index < m_pins.size(); index++)
            {
                std::cout << "New val: " << m_pins.at(index) << "\n";
            }
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
            return m_bus.getPosition();
        }

    private:
        int m_bits;
        sf::RectangleShape m_bus;
        float m_connection_height;
        bool m_interactable;
        std::vector<bool> m_pins;
        sf::RectangleShape m_pins_shape;
};

#endif /* CONNECTION_GUI */