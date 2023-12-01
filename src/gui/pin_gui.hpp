/** 
 * MIT License
 * 
 * Copyright (c) 2023 Ochawin A.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#ifndef PIN_GUI
#define PIN_GUI

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics.hpp>

// #include "wire_gui.hpp"
#include "../common.hpp"

constexpr float PIN_RADIUS = 15.f;

class PinGui 
{
public:

  PinGui(float pin_radius = PIN_RADIUS)
  : m_pin(pin_radius)
  , on { false }
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
      // std::cout << "Pin gui constructs with pin on\n";
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

  bool get_on()
  {
    return on;
  }

private:
  sf::CircleShape m_pin;
  bool on;
  bool            m_interactable;
};


#endif /* PIN_GUI */
