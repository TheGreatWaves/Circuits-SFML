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