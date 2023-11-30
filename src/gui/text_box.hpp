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
#ifndef TEXT_BOX
#define TEXT_BOX

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>

#include <algorithm>
#include <iostream>

#include "context.hpp"

inline float get_text_max_height(const sf::Text& l_text) 
{
	auto charSize = l_text.getCharacterSize();
	auto font = l_text.getFont();
	auto string = l_text.getString().toAnsiString();
	bool bold = (l_text.getStyle() & sf::Text::Bold);
	float max = 0.f;
	for (size_t i = 0; i < string.length(); ++i) 
	{
		sf::Uint32 character = string[i];
		auto glyph = font->getGlyph(character, charSize, bold);
		auto height = glyph.bounds.height;
		if (height <= max) 
		{ 
			continue; 
		}
		max = height;
	}
	return max;
}

class TextBoxGui
{
public:

	TextBoxGui(const std::string& default_str = "Unnamed", bool can_edit = true)
	: m_default_str(default_str)
	, m_can_edit(can_edit)
	{
		m_font.loadFromFile("resources/HelveticaNeueLTStd-It.otf");
		m_input_text = "";
		m_text = std::make_unique<sf::Text>(default_str, m_font);
		m_text->setCharacterSize(30);
		m_text->setFillColor(sf::Color::White);
		m_edit_mode = false;
	}

	void set_text_color(const sf::Color& color)
	{
		m_text->setFillColor(color);
	}

	void draw(sf::RenderTarget &target, sf::RenderStates states)
	{
		std::cout << "Text box text: " << std::string(m_text->getString()) << std::endl;
		target.draw(*m_text, states);
		std::cout << "Drawn the text" << std::endl;
	}

	void set_font_size(unsigned int size)
	{
		m_text->setCharacterSize(size);
	}

	void set_position(const sf::Vector2f pos)
	{
		m_text->setPosition(pos);
	}

	sf::Vector2f get_position()
	{
		return m_text->getPosition();
	}

	void set_string(const std::string& str)
	{
		m_text->setString(str);
	}

	void toggle_edit_mode() 
	{
		m_edit_mode = true;
	}

	void untoggle_edit_mode() 
	{
		m_edit_mode = false;
	}

	void handle_events(const sf::Event& event, bool no_box = true, 
					   const std::function<bool(const std::string&)>& condition = [](const std::string&) {return true;})
	{
		if (!m_can_edit) return;

		if (m_edit_mode)
		{
			Context::instance()->edit_mode = Mode::TEXT;
			m_text->setFillColor(sf::Color(100, 100, 100));
		}
		else
		{
			m_text->setFillColor(sf::Color::White);
		}

		const auto inside = (m_text->getGlobalBounds().contains(sf::Vector2f{static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)}));

		if (no_box && event.type == sf::Event::MouseButtonPressed)
		{
			m_edit_mode = inside;
		}
		else if (m_edit_mode && event.type == sf::Event::TextEntered)
		{
			if (std::isprint(event.text.unicode))
			{
				m_input_text += event.text.unicode;
				m_edited = true;
				if (!condition(m_input_text))
				{
					m_input_text.pop_back();
				}
			}
		}
		else if (m_edit_mode && event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::BackSpace)
			{
				if (!m_input_text.empty())
				{
					m_input_text.pop_back();
					m_edited = true;
				}
			}
			else if (event.key.code == sf::Keyboard::Return)
			{
				m_edit_mode = false;
				m_updated = true;
				Context::instance()->edit_mode = Mode::IDLE;
			}
		}

		if (m_edited)
		{
			m_text->setString(m_input_text);
			m_edited = false;
		}

		if (m_input_text.empty())
		{
			m_text->setString(m_default_str);
		}
	}

	const std::string& get_string()
	{
		return m_input_text;
	}

	float get_width()
	{
		return m_text->getGlobalBounds().width;
	}

	bool was_edited()
	{
		auto updated = m_updated;
		m_updated = false;
		return updated;
	}

	bool edited()
	{
		return m_edited;
	}
	
	float get_height()
	{
		return get_text_max_height(*m_text);
	}

	void reset()
	{
		m_text->setString(m_default_str);
		m_input_text = "";
	}

	void move(const sf::Vector2f& offset)
	{
		m_text->move(offset);
	}

private:
	bool                      m_can_edit;
	bool                      m_edit_mode;
	bool                      m_edited;	
	bool                      m_updated = false;
	sf::Font                  m_font;
	std::unique_ptr<sf::Text> m_text;
	std::string               m_input_text;
	std::string               m_default_str;
};


#endif /* TEXT_BOX */
