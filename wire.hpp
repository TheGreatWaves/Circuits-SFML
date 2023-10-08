#pragma once
#ifndef WIRE
#define WIRE

#include "pin.hpp"
#include "utils.hpp"

// Let's assume wires are one way.
struct Wire
{
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
    }
  }

  Pin* input;
  Pin* output;
};

#endif /* WIRE */
