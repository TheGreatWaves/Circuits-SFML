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

  void simulate()
  {
    if (output != nullptr)
    {
      output->state = input->state;
      output->simulate();
    }
  }

private:

  Pin* input;
  Pin* output;
};

#endif /* WIRE */
