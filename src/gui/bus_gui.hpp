#pragma once
#ifndef BUS_GUI
#define BUS_GUI

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics.hpp>

#include <string> 
#include "../common.hpp"

constexpr float BUS_HEIGHT = 15.f;
constexpr float BUS_WIDTH = 15.f;
constexpr int DEFAULT_BITS = 16;

class BusGui
{
public:
    BusGui(float bus_height = BUS_HEIGHT, float bus_width = BUS_WIDTH, int m_bits = DEFAULT_BITS)
    : m_bus(sf::Vector2(BUS_WIDTH, bus_height))
    , on { false }
    {
        m_bus.setFillColor(OFF_COLOR);
        m_bus.setOrigin({bus_width, bus_height});
        m_font.loadFromFile("resources/HelveticaNeueLTStd-It.otf");
        m_bits_text = std::make_unique<sf::Text>(std::to_string(m_bits), m_font);
        m_bits_text->setCharacterSize(10);
		m_bits_text->setFillColor(sf::Color::White);
    }

    bool contains(const sf::Vector2f& pos)
    {
        return m_bus.getGlobalBounds().contains(pos);
    }

    void handle_events(const sf::Event& event)
    {
        if (!m_interactable) return;

        if (event.type == sf::Event::MouseButtonPressed)
        {
            auto pressed = contains(sf::Vector2f{static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)});
            if (pressed)
            {
                on = !on;
            }
        }
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states)
    {
        if (on)
        {
            m_bus.setFillColor(ON_COLOR);
        }
        else
        {
            m_bus.setFillColor(OFF_COLOR);
        }
        target.draw(m_bus, states);
        target.draw(*m_bits_text, states);

    }

    void set_position(const sf::Vector2f& pos)
    {
        m_bus.setPosition(pos);
        m_bits_text->setPosition(pos);
    }

    [[ nodiscard ]] sf::Vector2f get_position() const
    {
        return m_bus.getPosition();
    }

    void set_interactability(bool interactable)
    {
        m_interactable = interactable;
    }


    bool is_on() const 
    {
        return on;
    }

    void toggle_on() 
    {
        on = true;
    }

    void toggle_off() 
    {
        on = false;
    }    

private:
    sf::RectangleShape m_bus;
    bool on = false;
    bool m_interactable;
    int m_bits = 0;
    sf::Font m_font;
	std::unique_ptr<sf::Text> m_bits_text;
};

#endif /* BUS__GUI */