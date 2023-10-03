#pragma once
#include <SFML/Window/Keyboard.hpp>
#ifndef BOARD_GUI
#define BOARD_GUI

#include <vector>

#include <SFML/Graphics.hpp>

#include "text_box.hpp"
#include "pin_port_gui.hpp"
#include "wire_gui.hpp"

class BoardGui 
{
public:
  BoardGui(const sf::Vector2f& size)
  : m_mode_text_box("Mode", false)
  {
    m_background.setSize(size);
    m_background.setFillColor(sf::Color(30, 30, 46));

    m_prototype.setFillColor(sf::Color(30, 30, 46));
    m_prototype.setOutlineColor(sf::Color(49, 50, 68));
    m_prototype.setOutlineThickness(2.f);
    m_prototype.setSize({size.x/1.1f, size.y/1.2f});
    m_prototype.setPosition((m_background.getSize() - m_prototype.getSize()) / 2.f);

    m_name_text_box.set_position({m_prototype.getPosition().x, 20.f});
    m_mode_text_box.set_position({m_prototype.getPosition().x + m_prototype.getSize().x / 1.2f, 20.f});

    auto prototype_position = m_prototype.getPosition();

    m_input_pin_port.set_size(m_prototype);
    m_input_pin_port.set_position({prototype_position.x-(m_input_pin_port.get_size().x / 2.f), prototype_position.y});
    m_input_pin_port.set_interactability(true);

    m_output_pin_port.set_interactability(false);
    m_output_pin_port.set_size(m_prototype);
    m_output_pin_port.set_position({prototype_position.x + m_prototype.getSize().x - (m_output_pin_port.get_size().x / 2.f), prototype_position.y});
  }

  void draw(sf::RenderTarget &target, sf::RenderStates states)
  {
    target.draw(m_background, states);
    target.draw(m_prototype, states);
    target.draw(m_name_text_box, states);
    target.draw(m_mode_text_box, states);

    for (auto& wire : m_wires)
    {
      wire.draw(target, states);
    }

    target.draw(m_input_pin_port, states);
    target.draw(m_output_pin_port, states);

  }

  void handle_keyboard(const sf::Event& event)
  {
    if (event.key.code == sf::Keyboard::Escape)
    {
      Context::instance()->edit_mode = Mode::IDLE;
      m_mode_text_box.set_string("Normal mode");
      m_mode_text_box.set_position({m_prototype.getPosition().x + m_prototype.getSize().x - m_mode_text_box.get_width(), 20.f});
    }
    else if (event.key.code == sf::Keyboard::E)
    {
      Context::instance()->edit_mode = Mode::WIRING;
      m_mode_text_box.set_string("Wiring mode");
      m_mode_text_box.set_position({m_prototype.getPosition().x + m_prototype.getSize().x - m_mode_text_box.get_width(), 20.f});
    }

  }

  void handle_events(const sf::Event& event)
	{
    if (event.type == sf::Event::KeyPressed) handle_keyboard(event);

    m_name_text_box.handle_events(event);

    for (auto& wire : m_wires)
    {
      wire.handle_events(event);
    }

    m_input_pin_port.handle_events(event);
    m_output_pin_port.handle_events(event);
	}

private:
  sf::RectangleShape m_background;
  sf::RectangleShape m_prototype;
  TextBoxGui         m_name_text_box;
  TextBoxGui         m_mode_text_box;

  // Temp, let's have one pin
  PinPortGui m_input_pin_port;
  PinPortGui m_output_pin_port;

  std::vector<WireGui> m_wires;

};

#endif /* BOARD_GUI */
