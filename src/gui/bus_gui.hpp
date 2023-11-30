#pragma once
#ifndef BUS_GUI
#define BUS_GUI

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics.hpp>

#include <string> 
#include "../common.hpp"
#include "text_box.hpp"
#include "pin_gui.hpp"

constexpr float BUS_HEIGHT = 30.f;
constexpr float BUS_WIDTH = 30.f;
constexpr int DEFAULT_BITS = 16;

class BusGui
{
public:

    BusGui(int front_index = 0, int m_bits = DEFAULT_BITS, float bus_height = BUS_HEIGHT, float bus_width = BUS_WIDTH)
    : m_bits { m_bits }
    , m_bus_height { m_bits*bus_height/4 }
    , on { false }
    , front_index { front_index }
    {
        m_bus = sf::RectangleShape(sf::Vector2(BUS_WIDTH, m_bus_height));
        // std::cout << "Creating bus\n";
        std::cout << "int bits: " << m_bits << "\n";
        const std::string bits_text = std::to_string(m_bits);
        m_total_bits_text_box.set_string(bits_text);
        m_total_bits_text_box.set_font_size(15);
        m_bus.setFillColor(OFF_COLOR);
        m_bus.setOrigin({(bus_width - 2), bus_height});
        // std::cout << "bus bits in constructor: " << bits_text << "\n";
        sf::Font m_font{};
        m_font.loadFromFile("resources/HelveticaNeueLTStd-It.otf");
        sf::Text temp_m_bits_text(bits_text, m_font);
        m_bits_text = temp_m_bits_text;
        m_bits_text.setCharacterSize(15);
		m_bits_text.setFillColor(sf::Color::White);
        for (int index = 0; index < m_bits; index++)
        {
            m_on_bits.emplace_back(false);
            bus_pins.emplace_back(sf::RectangleShape(sf::Vector2(BUS_WIDTH, m_bus_height/m_bits-2)));
        }
        member_pins_on_vals.emplace_back(&on);
    }

    bool contains(const sf::Vector2f& pos)
    {
        return m_bus.getGlobalBounds().contains(pos);
    }

    float get_width()
    {
        return BUS_WIDTH;
    }

    float get_height()
    {
        return m_bus_height;
    }

    void handle_events(const sf::Event& event)
    {
        if (!m_interactable) return;

        if (event.type == sf::Event::MouseButtonPressed)
        {
            // TODO: Update this so that member pins are switched on/off
            auto pressed = contains(sf::Vector2f{static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)});
            if (pressed)
            {
                on = !on;
            }
        }
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states)
    {
        // std::cout << "Draw called on bus\n";
        if (on)
        {
            m_bus.setFillColor(ON_COLOR);
        }
        else
        {
            m_bus.setFillColor(OFF_COLOR);
        }
        target.draw(m_bus, states);

        auto bus_pos = m_bus.getPosition();

        for (int index = 1; index < m_bits; index++)
        {
            // std::cout << "getting pin\n";
            auto current_pin = bus_pins[index];
            // std::cout << "got pin, getting pin val\n";
            if (member_pins_on_vals[index] == nullptr)
            {
                std::cout << "NULL pointer\n";
            }
            bool is_pin_on = *member_pins_on_vals[index];
            // std::cout << "setting pin color\n";
            if (is_pin_on)
            {
                current_pin.setFillColor(ON_COLOR);
            }
            else
            {
                current_pin.setFillColor(OFF_COLOR);
            }
            // std::cout << "Drawing bus_pin\n";
            current_pin.setPosition({bus_pos.x - 20, bus_pos.y - (index*m_bus_height/m_bits-2+2)});
            target.draw(current_pin, states);
        }

        // target.draw(m_bits_text, states);
        // m_total_bits_text_box.draw(target, states);
    }

    void set_position(const sf::Vector2f& pos)
    {
        m_bus.setPosition(pos);
        // m_total_bits_text_box.set_position({pos.x - 20, pos.y - 25});
        m_bits_text.setPosition({pos.x - 20, pos.y - 25});
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

    // TODO: Figure out a way to increment the bits switched on/off
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

    void add_member_pin(std::shared_ptr<bool> on_val)
    {
        if (on_val == nullptr)
        {
            std::cout << "null inputted\n";
        }
        else
        {
            std::cout << "member pointer: " << on_val << "\n";
        }
        member_pins_on_vals.emplace_back(on_val);
    }

private:
    sf::RectangleShape m_bus;
    float m_bus_height; 
	TextBoxGui m_on_bits_text_box; // TODO: Might switch to an interactable textbox
    TextBoxGui m_total_bits_text_box;
    bool on = false;
    bool m_interactable;
    int m_bits = 0;
    int front_index = 0;
    sf::Text m_bits_text;
    std::vector<sf::RectangleShape> bus_pins;
    std::vector<bool> m_on_bits {};
    std::vector<std::shared_ptr<bool>> member_pins_on_vals {};
};

#endif /* BUS__GUI */