#pragma once
#ifndef WIRE
#define WIRE

#include "pin.hpp"


// Let's assume wires are one way.
class Wire
{
public:

  void simulate()
  {
    output->state = input->state;
  }

private:

  explicit Wire(Pin* p1, Pin* p2)
  : input(p1)
  , output(p2)
  {
  }

  Pin* input;
  Pin* output;
};

#endif /* WIRE */
