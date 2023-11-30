#pragma once
#ifndef BUS_GUI
#define BUS_GUI

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics.hpp>

#include <string> 
#include "../common.hpp"

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
        m_bus.setFillColor(OFF_COLOR);
        m_bus.setOrigin({(bus_width - 2), bus_height});
        m_font.loadFromFile("resources/HelveticaNeueLTStd-It.otf");

        /*
        if (!m_font.loadFromFile("resources/HelveticaNeueLTStd-It.otf")) {
        // Font loading failed
        // You can handle this situation, e.g., by using a default font or logging an error
        std::cerr << "Failed to load font file!" << std::endl;
        // Consider loading a default font or handling the error gracefully
        }
        std::cout << "m_bits_text address after allocation: " << m_bits_text.get() << std::endl;
        if (m_bits_text) {
        // Log a message indicating successful initialization
        std::cout << "m_bits_text successfully initialized." << std::endl;
        } else {
            // Log an error message if initialization failed
            std::cerr << "Failed to initialize m_bits_text!" << std::endl;
        }
        */

        const std::string& default_str = std::to_string(m_bits);
        m_bits_text = std::make_unique<sf::Text>();
        m_bits_text->setFont(m_font);
        m_bits_text->setString(default_str);
        m_bits_text->setCharacterSize(10);
		m_bits_text->setFillColor(sf::Color::Black);
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
        std::cout << "Target.draw #1\n";
        target.draw(m_bus, states);
        // std::cout << "Target.draw #2\n";
        // target.draw(*m_bits_text, states);
        // std::cout << "Target.draw #2 completed\n";
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

    bool contains_pin_at_index(int index)
    {
        return ((front_index <= index) & ((front_index+m_bits-1) >= index));
    }

    bool is_first_pin(int index)
    {
        return front_index == index;
    }

private:
    sf::RectangleShape m_bus{};
	std::unique_ptr<sf::Text> m_bits_text;
    sf::Font m_font;
    bool on = false;
    bool m_interactable;
    int m_bits = 0;
    int front_index = 0;
};

#endif /* BUS__GUI */