#pragma once
#ifndef BUS_GUI
#define BUS_GUI

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics.hpp>

#include <string> 
#include "../common.hpp"
#include "text_box.hpp"

constexpr float BUS_HEIGHT = 30.f;
constexpr float BUS_WIDTH = 30.f;
constexpr int DEFAULT_BITS = 16;

class BusGui
{
public:
    BusGui(int front_index = 0, int m_bits = DEFAULT_BITS, float bus_height = BUS_HEIGHT, float bus_width = BUS_WIDTH)
    : m_bus(sf::Vector2(BUS_WIDTH, bus_height))
    , on { false }
    , m_bits { m_bits }
    , front_index { front_index }
    {
        std::cout << "Creating bus\n";
        std::cout << "int bits: " << m_bits << "\n";
        const std::string bits_text = std::to_string(m_bits);
        m_total_bits_text_box.set_string(bits_text);
        m_total_bits_text_box.set_font_size(15);
        m_bus.setFillColor(OFF_COLOR);
        m_bus.setOrigin({(bus_width - 2), bus_height});
        std::cout << "bus bits in constructor: " << bits_text << "\n";
    }

    bool contains(const sf::Vector2f& pos)
    {
        return m_bus.getGlobalBounds().contains(pos);
    }

    float get_width()
    {
        return BUS_WIDTH;
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
        m_total_bits_text_box.draw(target, states);
    }

    void set_position(const sf::Vector2f& pos)
    {
        m_bus.setPosition(pos);
        m_total_bits_text_box.set_position({pos.x - 20, pos.y - 25});
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

    bool contains_pin_at_index(int index)
    {
        return ((front_index <= index) & ((front_index+m_bits-1) >= index));
    }

    bool is_first_pin(int index)
    {
        return front_index == index;
    }

private:
    sf::RectangleShape m_bus;
	TextBoxGui m_on_bits_text_box; // TODO: Might switch to an interactable textbox
    TextBoxGui m_total_bits_text_box;
    bool on = false;
    bool m_interactable;
    int m_bits = 0;
    int front_index = 0;
};

#endif /* BUS__GUI */