#pragma once
#include <SFML/Graphics/Color.hpp>
#include <sys/syscall.h>
#ifndef WIRE_GUI
#define WIRE_GUI

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics.hpp>

#include "context.hpp"

class PinGui;

inline float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

constexpr float WIRE_GROWTH_RATE = 100.f;

class WireGui
{
public:

  WireGui()
  {
    m_circle.setRadius(2.f);
    m_circle.setOrigin({m_circle.getRadius(), m_circle.getRadius()});
    m_circle.setFillColor(sf::Color(88, 91, 112));

    m_pole.setRadius(5.f);
    m_pole.setOrigin({m_pole.getRadius(), m_pole.getRadius()});
    m_pole.setFillColor(sf::Color(150, 150 ,150));
  }

  void add_node(const sf::Vector2f& pos)
  {
    m_points.push_back(pos);
    m_max_reachable = static_cast<float>((m_points.size() - 1) * 100);
  }

  void handle_events(const sf::Event& event)
	{
    if (event.type == sf::Event::MouseButtonPressed)
    {
      if (event.mouseButton.button == sf::Mouse::Left)
      {
        add_node({static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)});
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

  void update(const sf::Time& dt);

  void set_src_pin(PinGui* src)
  {
    m_src = src;
  }

  void add_dest_pin(PinGui* dest)
  {
    m_dest_pins = dest;
  }

  void draw(sf::RenderTarget &target, sf::RenderStates states)
  {
    int count = 0;
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

        if (m_max_reachable > 1.f 
        && count >= static_cast<int>(m_reached_signal_start)
        && count <= static_cast<int>(m_reached_signal_end))
        {
          m_circle.setFillColor(sf::Color(220,20,60));
        }
        else
        {
          m_circle.setFillColor(sf::Color(88, 91, 112));
        }

        target.draw(m_circle, states);


        count += 1;
      }
    }

    for (const auto& p : m_points)
    {
      m_pole.setPosition(p);
      target.draw(m_pole);
    }
  }

  PinGui* get_src_pin()
  {
    return m_src;
  }

private:
  
  float                     m_reached_signal_start{-1.f};
  float                     m_reached_signal_end{-1.f};
  float                     m_max_reachable{};
  std::vector<sf::Vector2f> m_points{};
  sf::CircleShape           m_pole{};
  sf::CircleShape           m_circle{};
  PinGui*                   m_dest_pins{};
  PinGui*                   m_src{};
};


#endif /* WIRE_GUI */
