#pragma once
#ifndef D_SERIALIZER
#define D_SERIALIZER

#include <vector>
#include <map>
#include <fstream>

#include "pin.hpp"

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
  std::vector<Pin>       input_pins;
  std::vector<Pin>       output_pins;
  GateType                type;
  std::size_t                 subgate_count;
  std::string             name;

  std::map<std::size_t, Gate> subgates;

  explicit Gate(std::size_t ipc = 0,
                std::size_t opc = 0,
                GateType gate_type = GateType::CUSTOM,
                const std::string& gate_name = ""
    )
    : type{ gate_type }
    , name{ gate_name }
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

  std::size_t add_subgate(Gate* gate)
  {
    auto key = subgate_count++;
    subgates[key] = *gate;
    return key;
  }

/**
 * The following describes how I think the code should run.
 * 
 * 1. Loop through all input pins.
 * 2. Simulate connected wires. (collect all pins the inputs are connected to)
 * 3. Loop through all the pins found and simulate the components which they are attached to.
 * 4. Gather all of the output pins and repeat step 1. The simulation is completed once there
 *    is no longer any wire to travel across.
 */
  void handle_custom_type();

  /**
   * Built in type handlers.
   * TODO: Maybe abstract this out later.
   */
  void handle_and()
  {
    output_pins[0].state = (input_pins[0].is_active() && input_pins[1].is_active())
                         ? PinState::ACTIVE
                         : PinState::INACTIVE;
  }

  void handle_not()
  {
    output_pins[0].state = (input_pins[0].is_active() ? PinState::INACTIVE : PinState::ACTIVE);
  }
};

#include "wire.hpp"

inline void Gate::handle_custom_type()
{
  /**
   * Loop through all input pins and propagate signal.
   */

  for (auto& pins : input_pins)
  {
    for (auto& connection : pins.connections)
    {
      // Simulate all connections.
      connection.simulate();
    }
  }
}

#endif /* D_SERIALIZER */