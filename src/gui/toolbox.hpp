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
#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "../board.hpp"
#include "../lang/core/trie.hpp"
#include "component_gui.hpp"
#include "text_box.hpp"

class ToolBox
{
  public:
    ToolBox(const sf::Vector2f& pos)
        : m_search_text("search", true)
        , m_result_text("", false)
        , m_root_pos(sf::Vector2f{pos.x + 23.f, pos.y + 37.f})
    {
        m_selection_box.setFillColor(sf::Color::Transparent);
        m_selection_box.setOutlineThickness(2.f);
        m_selection_box.setSize(sf::Vector2f{TOOLBOX_WIDTH - TOOLBOX_X_MARGIN, 40.f});

        m_search_text.set_position(m_root_pos);
        m_search_text.set_font_size(18);
        m_result_text.set_font_size(18);
    }

    void set_position(const sf::Vector2f pos)
    {
        m_search_text.set_position(pos);
    }

    void set_string(const std::string& name)
    {
        std::cout << "Tool box string: " << name << '\n';
        m_result_text.set_string(name);
    }

    void update(const sf::Time& dt)
    {
        m_selection_box.setPosition(m_root_pos);
        m_selection_box.move(-(m_selection_box.getSize() / 2.f));

        int        active_index = 0;
        const auto mouse_pos    = sf::Mouse::getPosition(*Context::instance()->window);

        m_active_index = -1;
        for (const auto& entry : m_entries)
        {
            m_selection_box.move(sf::Vector2f{0.f, 45.f});

            if (m_selection_box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mouse_pos)))
            {
                m_active_index = active_index;
                break;
            }

            active_index++;
        }

        if (m_active_component != nullptr)
        {
          m_active_component->set_position(static_cast<sf::Vector2f>(mouse_pos));

          if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
          {
            m_active_component = nullptr;
          }
      
          return;
        }
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states)
    {
        m_selection_box.setOutlineColor(sf::Color(49, 50, 68));
        m_selection_box.setFillColor(sf::Color::Transparent);

        m_selection_box.setPosition(m_root_pos);
        m_selection_box.move(-(m_selection_box.getSize() / 2.f));

        m_search_text.set_position(m_selection_box.getPosition());
        m_search_text.move(sf::Vector2f{5.f, (m_selection_box.getSize().y / 2.f) - 13.f});

        target.draw(m_selection_box, states);
        m_search_text.draw(target, states);

        m_result_text.set_position(m_search_text.get_position());

        m_selection_box.setOutlineColor(sf::Color::Transparent);

        int active_index = 0;
        for (const auto& entry : m_entries)
        {
            set_string(entry->get_name());
            m_result_text.move(sf::Vector2f{0.f, 45.f});

            m_selection_box.move(sf::Vector2f{0.f, 45.f});

            set_hover(active_index==m_active_index);

            target.draw(m_selection_box, states);
            m_result_text.draw(target, states);

            active_index++;
        }

        if (m_active_component != nullptr)
        {
          m_active_component->draw(target, states);
        }
    }
    void set_hover(bool condition)
    {
        if (condition)
        {
            m_selection_box.setFillColor(sf::Color(49, 50, 68));
        }
        else
        {
            m_selection_box.setFillColor(sf::Color::Transparent);
        }
    }
    void handle_events(const sf::Event& event)
    {
        const auto mouse_pos    = sf::Mouse::getPosition(*Context::instance()->window);
        m_selection_box.setPosition(m_root_pos);
        m_selection_box.move(-(m_selection_box.getSize() / 2.f));


        const auto inside_box = m_selection_box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mouse_pos));
        const auto clicked = event.type == sf::Event::MouseButtonPressed;
        if (clicked)
        {
            if (inside_box)
            {
                m_search_text.toggle_edit_mode();
            }
            else
            {
                m_search_text.untoggle_edit_mode();
                set_idle();
            }

            if (m_active_component != nullptr)
            {
                place_component();
            }
            else if (m_active_component == nullptr && m_active_index != -1)
            {
                select_belt();
            }
        }

        const auto& m_query = m_search_text.get_string();
        m_search_text.handle_events(event, false);

        if (m_search_text.get_string() != m_query)
        {
            m_active_index = -1;
        }

        if (!m_search_text.get_string().empty())
        {
            m_entries = Board::instance()->search(m_search_text.get_string());
        }
        else
        {
            m_entries.clear();
        }
    }

    void place_component();

    void select_belt();

    // WARNING: This is bad.
    void set_idle();

  private:
    TextBoxGui                      m_search_text;
    TextBoxGui                      m_result_text;
    sf::RectangleShape              m_selection_box;
    std::vector<Gate*>              m_entries;
    int                             m_active_index = -1;
    const sf::Vector2f              m_root_pos;
    std::unique_ptr<ComponentGui>   m_active_component{};
};

#endif /* TOOLBOX_H */