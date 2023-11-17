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
#ifndef COMPONENT_GUI
#define COMPONENT_GUI

#include <string_view>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "../board.hpp"
#include "pin_gui.hpp"

#include "text_box.hpp"
#include "pin_port_gui.hpp"

constexpr float NAME_PADDING = 10.f;
constexpr float COMPONENT_PIN_RADIUS = 8.f;
const sf::Color BODY_COLOR = sf::Color(180, 190, 254);

class ComponentGui 
{
public:

  ComponentGui(std::string_view configuration_name)
  : m_name("", false)
  , m_input_pins(COMPONENT_PIN_RADIUS)
  , m_output_pins(COMPONENT_PIN_RADIUS)
  , m_component_name(configuration_name)
  {
    auto board = Board::instance();
    auto component = board->get_component(configuration_name);

    if (component != nullptr)
    {
      m_component = component->duplicate();

      auto input_count = m_component->input_pins.size();
      auto output_count = m_component->output_pins.size();
      auto max_side_pins_count = std::max(input_count, output_count);

      m_name.set_string(std::string{configuration_name});
      m_name.set_font_size(20);
      m_name.set_text_color(sf::Color(17, 17, 27));

      auto body_size = sf::Vector2f{ m_name.get_width() + NAME_PADDING + PIN_RADIUS * 2, max_side_pins_count * PIN_RADIUS * 2 };
      m_body.setSize(body_size);
      m_body.setFillColor(BODY_COLOR);
      m_body.setOrigin(m_body.getSize()/2.f);

      m_input_pins.set_interactability(true);
      m_output_pins.set_interactability(false);

      m_input_pins.setup_port(input_count);
      m_output_pins.setup_port(output_count);
    }
  }  

  void update(const sf::Time& dt)
  {
    if (m_component != nullptr)
    {

      // std::cout << BLOCK << " [ UPDATING COMPONENT: `" << m_component->name << "` ] " << BLOCK  << BLOCK << BLOCK << '\n';
      auto input_bits = m_input_pins.get_bits();    
      m_component->apply_input(m_input_pins.size(), input_bits);
      // m_component->input_info();
      // m_component->wire_info();
      // m_component->subgates_brief();

      m_component->simulate();
      auto output_bits = m_component->serialize_output();
      m_output_pins.apply_bits(output_bits);

      // m_component->output_info();
      // m_component->info();
      // std::cout << '\n';
    }
  }

  void set_position(const sf::Vector2f& pos)
  {
    m_body.setPosition(pos);
    m_input_pins.anchor(m_body);
    m_output_pins.anchor(m_body, false);
    auto body_size = m_body.getSize();

    auto body_gbounds = m_body.getGlobalBounds();
    auto text_pos = sf::Vector2f{body_gbounds.left + (body_size.x/2.f) - (m_name.get_width()/2.f), body_gbounds.top + (body_size.y/2.f) - (m_name.get_height())};
    m_name.set_position(text_pos);
  }
  
  void draw(sf::RenderTarget &target, sf::RenderStates states)
  {
    target.draw(m_body, states);
    m_input_pins.draw(target, states);
    m_output_pins.draw(target, states);
    target.draw(m_name, states);
  }

  void handle_events(const sf::Event& event)
	{
    m_input_pins.handle_events(event);
    m_output_pins.handle_events(event);
	}

  PinPortGui* get_input_pin_port()
  {
    return &m_input_pins;
  }

  PinPortGui* get_output_pin_port()
  {
    return &m_output_pins;
  }

  [[nodiscard]] const std::string& get_component_name() noexcept
  {
    return m_component_name;
  }

private:
  std::unique_ptr<Gate> m_component;
  sf::RectangleShape    m_body;
  TextBoxGui            m_name;
  PinPortGui            m_input_pins;
  PinPortGui            m_output_pins;
  std::string           m_component_name;
};

#endif /* COMPONENT_GUI */

