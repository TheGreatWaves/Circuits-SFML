#pragma once
#ifndef PIN
#define PIN

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
  // OPTIONAL: The pin may live on a gate, in which case we can simulate it.
  Gate* parent;

  Pin(Gate* p = nullptr)
  : state{ PinState::INACTIVE }
  , parent(p)
  {
  }

  void simulate();

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
