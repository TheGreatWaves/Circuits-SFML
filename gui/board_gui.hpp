#pragma once
#ifndef BOARD_GUI
#define BOARD_GUI

#include <vector>

#include <SFML/Graphics.hpp>

#include "text_box.hpp"

class BoardGui : public sf::Drawable
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
  }

  void draw(sf::RenderTarget &target, sf::RenderStates states) const override
  {
    target.draw(m_background, states);
    target.draw(m_prototype, states);
    target.draw(m_text_box, states);
  }

  void handle_events(const sf::Event& event)
	{
    m_text_box.handle_events(event);
	}

private:
  sf::RectangleShape m_background;
  sf::RectangleShape m_prototype;
  TextBoxGui         m_text_box;
};

#endif /* BOARD_GUI */
