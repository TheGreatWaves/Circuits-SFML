#pragma once
#ifndef CONNECTION_PORT_GUI
#define CONNECTION_PORT_GUI

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <iostream>
#include <vector>
#include "context.hpp"
#include "connection_gui.hpp"
#include "../lang/hdl/meta.hpp"

class ConnectionPortGui
{
    public:
        ConnectionPortGui()
        : m_connections{}
        , m_strip {}
        {
            m_strip.setFillColor(sf::Color::Transparent);
        }

        // TODO: Use vector.size() for pid. Essentially return pins and their ids so that when we call get_pins we can go through each pin and pid and call sketch->wire_pins
        std::pair<std::size_t, ConnectionGui*> get_connection(const sf::Vector2f& pos)
        {
            std::size_t index = 0;
            for (auto& connection : m_connections)
            {
                // Ugly and wasteful but it has to be done.
                if (connection.touches_box(pos))
                {
                    return { index, &connection }; // return the PID of the first pin and the rest of the pins
                }
                index += connection.get_number_of_pins();
            }
            return {0, nullptr};
        }

        // Assumes busses is sorted
        void setup_port(std::size_t size, std::vector<BusEntry> busses)
        {
            // Start from a blank slate.
            m_connections.clear();

            auto segments = static_cast<float>(size + 1);
            auto segment_size = m_strip.getSize().y / segments;

            std::size_t total_number_of_pins = 0;
            // std::cout << "Bus size: " << busses.size() << "\n";
            if (busses.size() == 0 && size > 0)
            {
                // std::cout << "Bus size is 0\n";
                std::size_t bits = size;
                while (bits --> 0)
                {
                    m_connections.emplace_back(1);
                    total_number_of_pins++;
                }
            }
            else
            {
                for (auto& bus: busses)
                {
                    auto starting_index = bus.start;
                    if (starting_index >= MAX_INPUT_PINS)
                    {
                        // Reducing the start index by max_pins_count to normalize output index.
                        starting_index -= MAX_INPUT_PINS;
                    }
                    if (starting_index + 1 > total_number_of_pins)
                    {
                        for (int index; index < starting_index + 1; index++)
                        {
                            // std::cout << "Starting index: " << starting_index << "\n";
                            // std::cout << "Total number of pins: " << total_number_of_pins << "\n";
                            // std::cout << "Adding pin\n";
                            if (starting_index + 1 == total_number_of_pins)
                            {
                                break;
                            }
                            m_connections.emplace_back(1);
                            total_number_of_pins++;
                        }
                    }
                    m_connections.emplace_back(bus.size);
                    total_number_of_pins += bus.size;
                }
            }

            if (total_number_of_pins != size)
            {
                std::cout << "Setup port did not setup the same number of pins as required\n";
            }

            float cumulative_height = 0.f;
            for (auto& connection: m_connections)
            {
                auto strip_pos = m_strip.getPosition();
                auto strip_size = m_strip.getSize();
                connection.set_position({strip_pos.x + (strip_size.x / 2.f), strip_pos.y + cumulative_height});
                cumulative_height += connection.get_connection_height();
            }
        }

        void set_size(const sf::RectangleShape& parent)
        {
            m_strip.setSize({10.f, parent.getSize().y});
        }


        void handle_events(const sf::Event& event, bool is_input = true)
	    {
            if (Context::instance()->edit_mode == Mode::WIRING)
            {
                return;
            }

            bool pressed = false;

            sf::Vector2f mouse_pos = {static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)};


            for (auto& connection : m_connections)
            {
                connection.handle_events(event);

                // Ugly and wasteful but it has to be done.
                if (connection.contains(mouse_pos))
                {
                    pressed = true;
                }
            }

            if (!pressed && event.type == sf::Event::MouseButtonPressed)
            {
                auto pressed = (m_strip.getGlobalBounds().contains(mouse_pos));

                if (pressed)
                {
                    add_connection(mouse_pos, is_input, Context::instance()->bus_bits);
                }
            }
        }

        void add_connection(const sf::Vector2f& pos, bool is_input = true, int bits = DEFAULT_CONNECTION_BITS);

        void draw(sf::RenderTarget &target, sf::RenderStates states) 
        {
            target.draw(m_strip, states);

            for (auto& connection : m_connections)
            {
                connection.draw(target, states);
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

        void set_pin_at_index(std::size_t index, bool value)
        {
            std::cout << "Setting pin at index: " << index << "\n";
            std::size_t current_index = index;
            std::size_t current_size = 0;
            for (auto& connection : m_connections)
            {
                current_size = connection.get_number_of_pins();
                if (index < current_size)
                {
                    // std::size_t position = index - current_size;
                    connection.set_pin(current_index, value);
                }
                else
                {
                    current_index -= current_size;
                }
            }
            // std::cout << "Done setting pin at index\n";
        }


        void apply_bits(std::size_t bits)
        {
            std::size_t working_bits = bits;
            auto count = get_number_of_pins();
            std::cout << "Count at apply_bits: " << count << "\n";
            std::cout << "Bits inputted: " << working_bits << "\n";
            std::size_t index = 0;
            while (count --> 0)
            {
                if ((working_bits >> count ) & 1) 
                {
                    // std::cout << "TRUE\n";
                    set_pin_at_index(index, true);
                }
                else
                {
                    // std::cout << "FALSE\n";
                    set_pin_at_index(index, false);
                }
            index++;
            }
        }

        void set_interactability(bool interactability)
        {
            m_interactable = interactability;
            for (auto& connection: m_connections)
            {
                connection.set_interactability(interactability);
            }
        }

        std::size_t get_number_of_pins()
        {
            std::size_t output = 0;
            for (auto& connection: m_connections)
            {
                output += connection.get_number_of_pins();
                // std::cout << "Output +=: " << connection.get_number_of_pins() << "\n";
                // std::cout << "m_connections len: " << m_connections.size() << "\n";
            }
            return output;
        }

        std::size_t get_bits()
        {
            std::size_t bits = 0;
            std::vector<bool> all_pins = {};
            for (auto& connection: m_connections)
            {
                std::vector<bool> pins = *connection.get_pins();
                std::size_t pins_size = pins.size();
                std::cout << "get_bits1\n";
                for (int index = 0; index < pins_size; index++)
                {
                    all_pins.push_back(pins.at(index)); // safe
                }
                std::cout << "get_bits2\n";
            }
            for (bool pin : all_pins)
            {
                bits <<= 1;
                bits |= (pin? 1 : 0);
            }
            return bits;
        }

        void clear_port()
        {
            m_connections.clear();
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

            auto size = get_number_of_pins();
            auto segments = static_cast<float>(size + 1);
            auto segment_size = m_strip.getSize().y / segments;
            float cumulative_height = 0.f;
            for (auto& connection: m_connections)
            {
                auto strip_pos = m_strip.getPosition();
                auto strip_size = m_strip.getSize();
                connection.set_position({strip_pos.x + (strip_size.x / 2.f), strip_pos.y + cumulative_height});
                cumulative_height += connection.get_connection_height();
            }
        }

    private:
        bool m_interactable = false;
        sf::RectangleShape  m_strip;
        std::vector<ConnectionGui> m_connections;
};

#endif /* CONNECTION_PORT_GUI */