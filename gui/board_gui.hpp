#pragma once
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
  {
    m_background.setSize(size);
    m_background.setFillColor(sf::Color(30, 30, 46));

    m_prototype.setFillColor(sf::Color(30, 30, 46));
    m_prototype.setOutlineColor(sf::Color(49, 50, 68));
    m_prototype.setOutlineThickness(2.f);
    m_prototype.setSize({size.x/1.1f, size.y/1.2f});
    m_prototype.setPosition((m_background.getSize() - m_prototype.getSize()) / 2.f);

    m_text_box.set_position({m_prototype.getPosition().x, 20.f});

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
    target.draw(m_text_box, states);

    target.draw(m_input_pin_port, states);
    target.draw(m_output_pin_port, states);

    m_wire.draw(target, states);
  }

  void handle_events(const sf::Event& event)
	{
    m_text_box.handle_events(event);
    m_input_pin_port.handle_events(event);
    m_output_pin_port.handle_events(event);
    m_wire.handle_events(event);
	}

private:
  sf::RectangleShape m_background;
  sf::RectangleShape m_prototype;
  TextBoxGui         m_text_box;

  // Temp, let's have one pin
  PinPortGui m_input_pin_port;
  PinPortGui m_output_pin_port;
  WireGui m_wire;

};

#endif /* BOARD_GUI */
