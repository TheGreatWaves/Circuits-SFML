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
#ifndef PIN
#define PIN

#include <vector>
#include <memory>
#include <map>

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
  // OPTIONAL: The pin may live on a gate, in which case we can simulate it.
  Gate* parent;

  Pin(Gate* p = nullptr)
  : state{ PinState::INACTIVE }
  , parent(p)
  {
  }

  void simulate(std::vector<bool>* visited = nullptr, std::map<std::size_t, std::unique_ptr<Gate>>* components = nullptr);

  [[nodiscard]] PinState get_state()
  {
    return state;
  }

  bool is_active()
  {
    return state == PinState::ACTIVE;
  }

  void reset()
  {
    state = PinState::INACTIVE;
  }

  void flip()
  {
    state = is_active() ? PinState::INACTIVE : PinState::ACTIVE;
  }

};



#endif /* PIN */
