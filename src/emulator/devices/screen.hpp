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
#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class Screen 
{
public:
 explicit Screen()
 : m_pixel({1.f, 1.f})
 {
 }

 auto draw(sf::RenderTarget& target, const std::array<uint16_t, 24577>& memory) -> void
 {
  auto y {-1};
  auto x {0};
  for (std::size_t i {0}; i < screen_memory_size; i++) {
   const auto address = base_offset + i;
   const auto value = memory[address];

   if ((i % 32) == 0)
   {
    x = 0;
    y += 1;
   }

   for (std::size_t j{0}; j < 16; j++)
   {
    m_pixel.setPosition(x, y);
    if ((value >> j) & 1) 
    {
     m_pixel.setFillColor(sf::Color::White);
    }
    else
    {
     m_pixel.setFillColor(sf::Color::Black);
    }
    x += 1;
    target.draw(m_pixel);
   }
  }
 }

 auto run() -> void
 {
  
 }

private:
 sf::RectangleShape m_pixel {};
 static constexpr std::size_t base_offset = 16384;
 static constexpr std::size_t screen_memory_size = 8192;
};




#endif /* SCREEN_HPP */
