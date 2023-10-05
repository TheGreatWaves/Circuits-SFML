#pragma once
#ifndef WIRE
#define WIRE

#include "pin.hpp"
#include "utils.hpp"

// Let's assume wires are one way.
class Wire
{
public:

  Wire(Pin* i, Pin* o)
  : input(i)
  , output(o)
  {}

  void simulate(std::vector<bool>* visited = nullptr, 
    std::map<std::size_t, std::unique_ptr<Gate>>* components = nullptr)
  {
    if (output != nullptr)
    {
      output->state = input->state;
      output->simulate(visited, components);
    }
  }

private:

  Pin* input;
  Pin* output;
};

#endif /* WIRE */
