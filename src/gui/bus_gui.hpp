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
    , member_pins{}
    {
        m_bus = sf::RectangleShape(sf::Vector2(BUS_WIDTH, m_bus_height));
        // std::cout << "Creating bus\n";
        std::cout << "int bits: " << m_bits << "\n";
        m_bus.setFillColor(OFF_COLOR);
        m_bus.setOrigin({(bus_width - 2), bus_height});
        // std::cout << "bus bits in constructor: " << bits_text << "\n";
        for (int index = 0; index < m_bits; index++)
        {
            bus_pins.emplace_back(sf::RectangleShape(sf::Vector2(BUS_WIDTH, m_bus_height/m_bits-2)));
        }
    }

    bool contains(int index, const sf::Vector2f& pos)
    {
        if (index > bus_pins.size())
        {
            return false;
        }
        return bus_pins.at(index).getGlobalBounds().contains(pos);
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
            for (int index = 0; index < bus_pins.size(); index++)
            {
                auto pressed = contains(index, sf::Vector2f{static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)});
                if (pressed)
                {
                    if (index == 0)
                    {
                        on = !on;
                    }
                    else
                    {
                        if (member_pins.at(index-1)->is_on())
                        {
                            member_pins.at(index-1)->toggle_off();
                        }
                        else
                        {
                            member_pins.at(index-1)->toggle_on();
                        }
                    }
                    std::cout << "Bus_pin pressed\n";
                }
            }

        }
    }

    int get_member_size()
    {
        return member_pins.size();
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states)
    {
        std::cout << "handle events: " << member_pins.size() << '\n';

        std::cout << this << '\n';
        target.draw(m_bus, states);

        auto bus_pos = m_bus.getPosition();

        // index 0 below is actually index 1 in bus because we store index 0 in this bus class and it has its own on/off.
        for (int index = 0; index < m_bits; index++)
        {
            bool is_pin_on;
            std::cout << "Index: " << index << " Bus_pins size: " << bus_pins.size() << "\n";
            auto& current_pin_rec = bus_pins.at(index);
            if (index == 0)
            {
                is_pin_on = is_on();
            }
            else
            {
                std::cout << "getting pin\n";
                std::cout << "got bus_pin, getting pin val\n";
                if (member_pins.at(index-1) == nullptr)
                {
                    std::cout << "NULL pointer\n";
                }
                std::cout << "got pin\n";
                is_pin_on = member_pins.at(index-1)->is_on();
            }

            if (is_pin_on)
            {
                current_pin_rec.setFillColor(ON_COLOR);
            }
            else
            {
                current_pin_rec.setFillColor(OFF_COLOR);
            }
            std::cout << "Drawing bus_pin\n";
            current_pin_rec.setPosition({bus_pos.x - 20, bus_pos.y - ((index)*m_bus_height/m_bits-2+2)});
            target.draw(current_pin_rec, states);
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

    bool is_first_pin(int index)
    {
        return front_index == index;
    }

    void add_member_pin(PinGui* pin)
    {
        std::cout << this << '\n';
        if (pin == nullptr)
        {
            std::cout << "null inputted\n";
        }
        else
        {
            std::cout << "Member pins size before: " << member_pins.size() << "\n";
            std::cout << "Adding member pointer: " << pin << "\n";
            member_pins.push_back(pin); // TODO: Why does the size of the vector go down even after adding a new value
            std::cout << "Member pins size now: " << member_pins.size() << "\n";
        }
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
    std::vector<PinGui*> member_pins;
};

#endif /* BUS__GUI */