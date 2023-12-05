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
#ifndef WIRE_GUI
#define WIRE_GUI

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <sys/syscall.h>

#include "context.hpp"
#include "connection_gui.hpp"
#include "../common.hpp"


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

  void set_src_pin(ConnectionGui* connection, std::size_t index)
  {
    m_src = { connection, index };
  }

  void set_src_index(std::size_t idx)
  {
    m_src_idx = idx;
  }

  void set_dest_index(std::size_t idx)
  {
    m_dest_idx = idx;
  }

  std::size_t get_src_index() const
  {
    return m_src_idx;
  }

  std::size_t get_dest_index() const
  {
    return m_dest_idx;
  }

  void set_dest_pin(ConnectionGui* connection, std::size_t index)
  {
    m_dest_pins = { connection, index };
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

  std::pair<ConnectionGui*, std::size_t>  get_src_pin()
  {
    return m_src;
  }
  
  std::pair<ConnectionGui*, std::size_t>  get_dest_pin()
  {
    return m_dest_pins;
  }

  void set_src_connection_idx(std::size_t val)
  {
    m_src_connection_idx = val;
  }

  void set_dest_connection_idx(std::size_t val)
  {
    m_dest_connection_idx = val;
  }

  std::size_t get_src_connection_idx()
  {
    return m_src_connection_idx;
  }

  std::size_t get_dest_connection_idx()
  {
    return m_dest_connection_idx;
  }

  std::size_t get_src_connection_size()
  {
    return m_src.first->get_number_of_pins();
  }

  std::size_t get_dest_connection_size()
  {
    return m_dest_pins.first->get_number_of_pins();
  }

private:
  
  float                     m_reached_signal_start{-1.f};
  float                     m_reached_signal_end{-1.f};
  float                     m_max_reachable{};
  std::vector<sf::Vector2f> m_points{};
  sf::CircleShape           m_pole{};
  sf::CircleShape           m_circle{};
  std::pair<ConnectionGui*, std::size_t>                   m_dest_pins{};
  std::pair<ConnectionGui*, std::size_t>                   m_src{};

  std::size_t m_src_idx{};
  std::size_t m_dest_idx{};

  std::size_t m_src_connection_idx{};
  std::size_t m_dest_connection_idx{};
};


#endif /* WIRE_GUI */
