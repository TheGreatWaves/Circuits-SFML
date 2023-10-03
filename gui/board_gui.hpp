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
  }

  void draw(sf::RenderTarget &target, sf::RenderStates states) const override
  {
    target.draw(m_background, states);
    target.draw(m_text_box, states);
  }

  void handle_events(const sf::Event& event)
	{
    m_text_box.handle_events(event);
	}

private:
  sf::RectangleShape m_background;
  TextBoxGui         m_text_box;
};

#endif /* BOARD_GUI */
