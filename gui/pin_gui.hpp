#pragma once
#ifndef PIN_GUI
#define PIN_GUI

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics.hpp>

#include "wire_gui.hpp"

const sf::Color ON_COLOR = sf::Color(220,20,60);
const sf::Color OFF_COLOR = sf::Color(88, 91, 112);

class PinGui 
{
private:
  
public:

  PinGui()
  : m_pin(15.f)
  , on{ false }
  {
    m_pin.setFillColor(OFF_COLOR);
    m_pin.setOrigin({m_pin.getRadius(), m_pin.getRadius()});
  }
  
  bool contains(const sf::Vector2f& pos)
  {
    return m_pin.getGlobalBounds().contains(pos);
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
      m_pin.setFillColor(ON_COLOR);
    }
    else
    {
      m_pin.setFillColor(OFF_COLOR);
    }
    target.draw(m_pin, states);
  }

  void set_position(const sf::Vector2f& pos)
  {
    m_pin.setPosition(pos);
  }

  [[ nodiscard ]] sf::Vector2f get_position() const
  {
    return m_pin.getPosition();
  }

  float get_radius() const
  {
    return m_pin.getRadius();
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
  sf::CircleShape m_pin;
  bool            on = false;
  bool            m_interactable;
};


#endif /* PIN_GUI */
