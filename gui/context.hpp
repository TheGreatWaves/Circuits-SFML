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

/**
 * Dealing with a bunch of shared references is annoying,
 * since I'm making a small basic program, I'll just use a
 * simple singleton which contains all the information I need.
 */
#pragma once
#ifndef GUI_CONTEXT
#define GUI_CONTEXT

#include <SFML/Graphics.hpp>

#include <string_view>
#include <memory>

enum class Mode
{
 IDLE,
 WIRING,
 TEXT,
};

// Forwarding wire class
class WireGui;
class ComponentGui;
class BoardGui;
class Gate;

struct Context
{
  /**
   * Data members.
   */
  std::string_view              current_component_name;
  Mode                          edit_mode = Mode::IDLE;
  WireGui*                      active_wire = nullptr;
  sf::RenderWindow*             window = nullptr;
  BoardGui*                     board = nullptr;
  Gate*                         sketch = nullptr;

  /**
   * Yes, I should throw an error if the constructor is called again, but I'm too lazy!
   */
  Context() { singleton = this; }
  static Context* instance() { return singleton; }
  static Context* singleton;
};

inline Context* Context::singleton = nullptr;

#endif /* GUI_CONTEXT */
