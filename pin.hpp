#pragma once
#ifndef PIN
#define PIN

#include <vector>

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

  // OPTIONAL: The pin may live on a gate, in which case we can simulate it.
  Gate* parent;

  [[nodiscard]] PinState get_state()
  {
    return state;
  }

  bool is_active()
  {
    return state == PinState::ACTIVE;
  }

  std::vector<Wire> connections;
};

#endif /* PIN */
