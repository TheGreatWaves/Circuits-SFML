#pragma once
#include <SFML/Graphics/Color.hpp>
#ifndef PIN_GUI
#define PIN_GUI

#include <SFML/Graphics.hpp>

const sf::Color ON_COLOR = sf::Color(220,20,60);
const sf::Color OFF_COLOR = sf::Color(88, 91, 112);

class PinGui : public sf::Drawable
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

    if (on)
    {
      m_pin.setFillColor(ON_COLOR);
    }
    else
    {
      m_pin.setFillColor(OFF_COLOR);
    }
	}

  void draw(sf::RenderTarget &target, sf::RenderStates states) const override
  {
    target.draw(m_pin, states);
  }

  void set_position(const sf::Vector2f& pos)
  {
    m_pin.setPosition(pos);
  }

  float get_radius() const
  {
    return m_pin.getRadius();
  }

  void set_interactability(bool interactable)
  {
    m_interactable = interactable;
  }

private:
  sf::CircleShape m_pin;
  bool            on;
  bool            m_interactable;
};


#endif /* PIN_GUI */
