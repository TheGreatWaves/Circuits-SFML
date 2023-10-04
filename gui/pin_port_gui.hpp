#pragma once
#ifndef PIN_PORT_GUI
#define PIN_PORT_GUI

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <vector>
#include "pin_gui.hpp"

class PinPortGui 
{
public:

  PinPortGui(float pin_radius = PIN_RADIUS)
  : m_pin_rad(pin_radius)
  {
    m_strip.setFillColor(sf::Color::Transparent);
  }

  void set_interactability(bool interactable)
  {
    m_interactable = interactable;
  }

  std::size_t get_bits()
  {
    std::size_t bits = 0;
    for (auto& pin : m_pins)
    {
      bits <<= 1;
      bits |= (pin.is_on() ? 1 : 0);
    }
    return bits;
  }

  void apply_bits(std::size_t bits)
  {
    auto count = m_pins.size();
    std::size_t index = 0;
    while (count --> 0)
    {
      if ((bits >> count ) & 1) 
      {
        m_pins[index].toggle_on();
      }
      else
      {
        m_pins[index].toggle_off();
      }
      index++;
    }
  }

  void setup_port(std::size_t size)
  {
    // Start from a blank slate.
    m_pins.clear();

    auto segments = static_cast<float>(size + 1);
    auto segment_size = m_strip.getSize().y / segments;

    for (std::size_t i = 0; i < size; i++)
    {
      m_pins.emplace_back(m_pin_rad);
      auto& pin = m_pins.back();
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

    auto size = m_pins.size();
    auto segments = static_cast<float>(size + 1);
    auto segment_size = m_strip.getSize().y / segments;

    for (std::size_t i = 0; i < size; i++)
    {
      auto& pin = m_pins[i];
      auto strip_pos = m_strip.getPosition();
      auto strip_size = m_strip.getSize();
      pin.set_position({strip_pos.x + (strip_size.x / 2.f), strip_pos.y + ((i + 1) * segment_size)});
    }
  }

  std::size_t size()
  {
    return m_pins.size();
  }

  void clear_port()
  {
    m_pins.clear();
  }

  std::pair<std::size_t, PinGui*> get_pin(const sf::Vector2f& pos)
  {
    std::size_t index = 0;
    for (auto& p : m_pins)
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

  void set_size(const sf::RectangleShape& parent)
  {
    m_strip.setSize({10.f, parent.getSize().y});
  }
  
  void handle_events(const sf::Event& event)
	{
    if (Context::instance()->edit_mode == Mode::WIRING)
    {
      return;
    }

    bool pressed_pin = false;

    sf::Vector2f mouse_pos = {static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)};


    for (auto& p : m_pins)
    {
      p.handle_events(event);

      // Ugly and wasteful but it has to be done.
      if (p.contains(mouse_pos))
      {
        pressed_pin = true;
      }
    }

    if (!pressed_pin && event.type == sf::Event::MouseButtonPressed)
    {
	    auto pressed = (m_strip.getGlobalBounds().contains(mouse_pos));

      if (pressed)
      {
        m_pins.emplace_back(m_pin_rad);
        m_pins.back().set_interactability(m_interactable);
        m_pins.back().set_position({m_strip.getPosition().x + (m_strip.getSize().x/2.f), mouse_pos.y});
      }
    }
	}

  void draw(sf::RenderTarget &target, sf::RenderStates states) 
  {
    target.draw(m_strip, states);

    for (auto& p : m_pins)
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
  std::vector<PinGui> m_pins;
};

#endif /* PIN_PORT_GUI */
