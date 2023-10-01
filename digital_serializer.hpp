#pragma once
#ifndef D_SERIALIZER
#define D_SERIALIZER

#include <vector>
#include <fstream>

// List of built in types.
enum class GateType
{
  NOT,
  AND,
  CUSTOM
};

// Built in chips.
struct Gate
{
  std::vector<bool> input_pins;
  std::vector<bool> output_pins;
  GateType          type;

  explicit Gate(std::size_t ipc = 0, std::size_t opc = 0, GateType gate_type = GateType::CUSTOM)
  : type{ gate_type }
  {
    input_pins.resize(ipc);
    output_pins.resize(opc);
  }

  // Built in handlers for AND and NOT gates

  void simulate()
  {
    switch (type)
    {
    break; case GateType::NOT: handle_not();
    break; case GateType::AND: handle_and();
    break; case GateType::CUSTOM: handle_custom_type();
    }
  }

  void handle_custom_type()
  {
    /**
     * The following describes how I think the code should run.
     * 
     * 1. Loop through all input pins.
     * 2. Simulate connected wires. (collect all pins the inputs are connected to)
     * 3. Loop through all the pins found and simulate the components which they are attached to.
     * 4. Gather all of the output pins and repeat step 1. The simulation is completed once there
     *    is no longer any wire to travel across.
     */
  }

  /**
   * Built in type handlers.
   * TODO: Maybe abstract this out later.
   */
  void handle_and()
  {
    output_pins[0] = (input_pins[0] && input_pins[1]);
  }

  void handle_not()
  {
    output_pins[0] = !(input_pins[0]);
  }
};

#endif /* D_SERIALIZER */