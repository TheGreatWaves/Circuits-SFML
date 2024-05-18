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
#ifndef PIN_H
#define PIN_H

#include <vector>
#include <memory>

class Wire;
class Gate;

enum class PinState
{
  INACTIVE,
  ACTIVE
};

struct Pin
{
  PinState state;
  std::vector<std::shared_ptr<Wire>> connections;
  Gate* parent;

  Pin(Gate* p = nullptr)
  : state{ PinState::INACTIVE }
  , parent(p)
  {
  }

  ~Pin() = default;

  [[nodiscard]] PinState get_state()
  {
    return state;
  }

  bool is_active() const
  {
    return state == PinState::ACTIVE;
  }

  void set_on()
  {
    state = PinState::ACTIVE;
  }

  void set_off()
  {
    state = PinState::INACTIVE;
  }

  void reset()
  {
    set_off();
  }

  void flip()
  {
    state = is_active() ? PinState::INACTIVE : PinState::ACTIVE;
  }

  bool has_parent()
  {
    return parent != nullptr;
  }

};



#endif /* PIN_H */
