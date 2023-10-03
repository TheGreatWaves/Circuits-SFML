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
 
