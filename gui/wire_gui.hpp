#pragma once
#include <SFML/Graphics/Color.hpp>
#ifndef WIRE_GUI
#define WIRE_GUI

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics.hpp>

#include "context.hpp"

inline float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

class WireGui
{
public:

  WireGui()
  {
    m_circle.setRadius(2.f);
    m_circle.setOrigin({m_circle.getRadius(), m_circle.getRadius()});

    m_pole.setRadius(5.f);
    m_pole.setOrigin({m_pole.getRadius(), m_pole.getRadius()});
    m_pole.setFillColor(sf::Color(150, 150 ,150));
  }

  void handle_events(const sf::Event& event)
	{
    if (event.type == sf::Event::MouseButtonPressed)
    {
      if (event.mouseButton.button == sf::Mouse::Left)
      {
        m_points.push_back({static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)});
      }
      else if (event.mouseButton.button == sf::Mouse::Right)
      {
        auto mouse_pos = sf::Vector2f{static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)};
        for (std::size_t i = 0; i < m_points.size(); i++)
        {
          m_pole.setPosition(m_points[i]);
          if (m_pole.getGlobalBounds().contains(mouse_pos))
          {
            while(m_points.size() != i)
            {
              m_points.pop_back();
            }
          }
        }
      }
    }
	}  


  void draw(sf::RenderTarget &target, sf::RenderStates states)
  {
    for (int i = 0; i < static_cast<int>(m_points.size()) - 1; i++)
    {
      const auto& p1 = m_points[i];
      const auto& p2 = m_points[i+1];

      const float steps = 100.f;

      for (std::size_t i = 0; i < static_cast<std::size_t>(steps); i++)
      {
        auto x = lerp(p1.x, p2.x, static_cast<float>(i) / steps);
        auto y = lerp(p1.y, p2.y, static_cast<float>(i) / steps);
        
        m_circle.setPosition({x, y});
        target.draw(m_circle, states);
      }
    }

    for (const auto& p : m_points)
    {
      m_pole.setPosition(p);
      target.draw(m_pole);
    }
  }

private:
  std::vector<sf::Vector2f> m_points;
  sf::CircleShape m_pole;
  sf::CircleShape m_circle;
};

#endif /* WIRE_GUI */
