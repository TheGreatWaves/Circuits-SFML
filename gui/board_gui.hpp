#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "../application/Object.hpp"

class BoardGui : public Object
{
public:

  BoardGui(sf::RenderTarget& t)
  : target(t)
  {
    auto size = static_cast<sf::Vector2f>(t.getSize());
    size.y *= 0.90;
    board.setSize(size);

    board.setFillColor(sf::Color(150, 150, 150));
  }

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
  {
    target.draw(board, states);
  }
  
private:
  sf::RectangleShape board;
  sf::RenderTarget& target;

  const bool shapeContainsPoint(const Mouse &mouse) const noexcept override
  {
    return board.getGlobalBounds().contains(mouse.position());
  }
};
