#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#ifndef PIN_PORT_GUI
#define PIN_PORT_GUI

#include <SFML/Graphics.hpp>

#include <vector>
#include "pin_gui.hpp"

class PinPortGui 
{
public:

  PinPortGui()
  {
    m_strip.setFillColor(sf::Color::Transparent);
  }

  void set_interactability(bool interactable)
  {
    m_interactable = interactable;
  }

  PinGui* get_pin(const sf::Vector2f& pos)
  {
    for (auto& p : m_pins)
    {
      // Ugly and wasteful but it has to be done.
      if (p.contains(pos))
      {
        return &p;
      }
    }
    return nullptr;
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
        m_pins.emplace_back();
        auto r = m_pins.back().get_radius();
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
  sf::RectangleShape  m_strip;
  std::vector<PinGui> m_pins;
};

#endif /* PIN_PORT_GUI */
