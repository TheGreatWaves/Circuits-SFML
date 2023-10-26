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

#include "wire_gui.hpp"
#include "pin_gui.hpp"

#include <iostream>

void WireGui::update(const sf::Time& dt)
{
  if (m_src != nullptr)
  {
    if (m_src->is_on())
    {
      // Kick start a signal.
      if (m_reached_signal_end < 0.f)
      {
        m_reached_signal_end = 0.1f;
      }
    }
    else
    {
      // Kick start a reap signal.
      if (m_reached_signal_start < 0.f && m_reached_signal_end >= 0.f)
      {
        m_reached_signal_start = 0.1f;
      }
    }

  }

  // The signal will reach until the end.
  if (m_reached_signal_end >= 0.f && m_reached_signal_end < m_max_reachable)
  {
    m_reached_signal_end += (WIRE_GROWTH_RATE * dt.asSeconds());
  }


  if (m_reached_signal_end >= m_max_reachable)
  {
    if (m_dest_pins != nullptr)
    {
      m_dest_pins->toggle_on();
    }
  }

  if (m_reached_signal_start >= 0.f && m_reached_signal_start < m_max_reachable)
  {
    m_reached_signal_start += (WIRE_GROWTH_RATE * dt.asSeconds());
  }

  if (m_reached_signal_start >= m_max_reachable)
  {
    if (m_dest_pins != nullptr)
    {
      m_dest_pins->toggle_off();
    }

    m_reached_signal_end = -1.f;
    m_reached_signal_start = -1.f;
  }
}
 
