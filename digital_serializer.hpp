#pragma once
#include <utility>
#ifndef D_SERIALIZER
#define D_SERIALIZER

#include <vector>
#include <map>
#include <fstream>

#include "pin.hpp"
#include "utils.hpp"

#define INPUT_PIN_LIMIT 100

// List of built in types.
enum class GateType
{
  NAND,
  CUSTOM
};

// Built in chips.
struct Gate
{
  std::vector<Pin>                             input_pins;
  std::vector<Pin>                             output_pins;
  std::size_t                                  pin_count;
  GateType                                     type;
  std::size_t                                  subgate_count;
  std::string                                  name;
  std::map<std::size_t, std::unique_ptr<Gate>> subgates;

  bool                                         serialized;
  std::vector<std::size_t>                     serialized_computation;

  explicit Gate(std::size_t ipc = 0,
                std::size_t opc = 0,
                GateType gate_type = GateType::CUSTOM,
                const std::string& gate_name = "",
                bool is_serialized = false
    )
    : type{ gate_type }
    , input_pins(ipc, Pin(this))
    , output_pins(opc, Pin())
    , name{ gate_name }
    , serialized{ is_serialized }
  {
  }

  void serialize()
  {
    std::size_t indicies = (2 << (input_pins.size() - 1));

    for (std::size_t i = 0; i < indicies; i++)
    {
      apply_input(input_pins.size(), i);

      simulate();

      for (std::size_t n = 0; n < input_pins.size(); n++)
      {
        log(n, ' ');
      }

      log("| ");

      for (std::size_t n = 0; n < output_pins.size(); n++)
      {
        log(n, ' ');
      }

      newline();
      for (std::size_t n = 0; n < (input_pins.size() + output_pins.size()); n++)
      {
        log("==");
      }
      log("=");
      newline();

      for (std::size_t j = 0; j < input_pins.size(); j++)
      {
        log((input_pins[j].state == PinState::ACTIVE) ? 1 : 0, ' ');
      }

      log("| ");

      for (std::size_t j = 0; j < output_pins.size(); j++)
      {
        log((output_pins[j].state == PinState::ACTIVE) ? 1 : 0, ' ');
      }

      newline();

      serialized_computation.push_back(serialize_output());
    }

    serialized = true;
  }

  std::size_t serialize_output()
  {
    std::size_t so = 0;
    for (std::size_t j = 0; j < output_pins.size(); j++)
    {
      so <<= 1;
      so |= (output_pins[j].is_active() ? 1 : 0);
    }
    return so;
  }

  std::size_t serialize_input()
  {
    std::size_t so = 0;
    for (std::size_t j = 0; j < input_pins.size(); j++)
    {
      so <<= 1;
      so |= (input_pins[j].is_active() ? 1 : 0);
    }
    return so;
  }

  void simulate_serialized()
  {
    // Get serialized entry of the input.
    auto serialized_input = serialize_input();

    // Retrieve the serialized output entry using the serialized input.
    auto serialized_output = serialized_computation[serialized_input];

    // Apply the serialized output to the output pins.
    apply_output(static_cast<int>(output_pins.size()), serialized_output);
  }

  void apply_input(int count, std::size_t mask)
  {
    std::size_t index = 0;
    while (count --> 0)
    {
      input_pins[index++].state = (((mask >> count ) & 1) == 1) ? PinState::ACTIVE : PinState::INACTIVE;
    }

    newline();
  }

  void apply_output(int count, std::size_t mask)
  {
    std::size_t index = 0;
    while (count --> 0)
    {
      std::cout << ((mask >> count) & 1);
      output_pins[index++].state = (((mask >> count ) & 1) == 1) ? PinState::ACTIVE : PinState::INACTIVE;
    }

    newline();
  }

  void simulate()
  {
    switch (type)
    {
    break; case GateType::NAND: handle_nand();
    break; case GateType::CUSTOM: handle_custom_type();
    break; default: log("Invalid type...?\n");
    }

    for (auto& pins : output_pins)
    {
      pins.simulate();
    }
  }


  void reset()
  {
		log("Resetting output pins...\n");
    for (auto& output_pin : output_pins)
    {
      output_pin.reset();
    }
  }

  void add_input_pin()
  {
    pin_count++;
    input_pins.emplace_back();
  }

  void add_output_pin()
  {
    pin_count++;
    output_pins.emplace_back();
  }

  Pin* get_pin(std::size_t pin)
  {
    if (INPUT_PIN_LIMIT > pin)
    {
      std::size_t range = input_pins.size();

      if ( range > pin )
      {
        return &input_pins[pin];
      }
      else
      {
        for (auto& subgate : subgates)
        {
          auto s = subgate.second->input_pins.size();
          range += s;
          if ( range > pin )
          {
            range -= s;
            return &subgate.second->input_pins.at(pin - range);
          }
        }
      }
    }
    else
    {
      pin -= INPUT_PIN_LIMIT;

      std::size_t range = output_pins.size();

      if ( range > pin )
      {
        auto w = &output_pins[pin];
        return w;
      }
      else
      {
        for (auto& subgate : subgates)
        {
          auto s = subgate.second->output_pins.size();
          range += s;
          if ( range > pin )
          {
            range -= s;
            return &subgate.second->output_pins.at(pin - range);
          }
        }
      }
    }

    return nullptr;
  }

  bool wire_pins(std::size_t p1, std::size_t p2)
  {
    auto pa = get_pin(p1);

    if (pa == nullptr)
    {
      return false;
    }

    auto pb = get_pin(p2);

    if (pb == nullptr)
    {
      return false;
    }

    return connect_pins(pa, pb);
  }

  bool connect_pins(Pin* input, Pin* output);

  bool has_pin(std::size_t input_id)
  {
    return input_pins.size() > input_id;
  }

  PinState get_pin_state(std::size_t input_id)
  {
    return input_pins.at(input_id).state;
  }

  bool toggle_pin(std::size_t input_id)
  {
    if (input_pins.size() < input_id) return false;
    input_pins[input_id].flip();
    return true;
  }

  std::size_t add_subgate(Gate* gate)
  {
    auto key = subgate_count++;
    subgates[key] = std::make_unique<Gate>(gate->duplicate());

    for (auto& p : subgates[key]->input_pins)
    {
      p.parent = subgates[key].get();
    }

    return key;
  }

  Gate duplicate()
  {
    Gate g(input_pins.size(), output_pins.size(), this->type, this->name, this->serialized);
    g.serialized_computation = this->serialized_computation;
    return g;
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
   * Built-in type handlers.
   * TODO: Maybe abstract this out later.
   */
  void handle_nand()
  {
    output_pins[0].state = (!(input_pins[0].is_active() && input_pins[1].is_active()))
                         ? PinState::ACTIVE
                         : PinState::INACTIVE;
  }
  
  // Prints information about the current gate.
  void info()
  {
    log(BLOCK, " Input Pins ", BLOCK, '\n');
		auto count = 0;
		for (const auto& pin : input_pins)
		{
			log("pin[", count, "] ", pin.state == PinState::ACTIVE ? 1 : 0, "\n");
			count++;
		}

    auto output_count = 0;
    log(BLOCK, " Output Pins ", BLOCK, '\n');
		for (const auto& pin : output_pins)
		{
			log("pin[", output_count + INPUT_PIN_LIMIT, "]\n");
			output_count++;
		}

    for (const auto& subgate : subgates)
    {
      newline();
      log(BLOCK, " Subgate[", subgate.first, "] ", BLOCK, "\n");
  		for (const auto& pin : subgate.second->input_pins)
  		{
  			log("    pin[", count, "]\n");
  			count++;
  		}
      log("Exposed output pins:\n");
  		for (const auto& pin : subgate.second->output_pins)
  		{
  			log("    pin[", output_count+INPUT_PIN_LIMIT, "]\n");
  			output_count++;
  		}
    }
  }
};

#include "wire.hpp"

inline void Gate::handle_custom_type()
{
  if (serialized)
  {
    simulate_serialized();
    return;
  }
  /**
   * Loop through all input pins and propagate signal.
   */
  for (auto& pins : input_pins)
  {
    pins.simulate();
  }
}

inline bool Gate::connect_pins(Pin* input, Pin* output)
{
  if (input == nullptr || output == nullptr) return false;
  input->connections.push_back(std::make_shared<Wire>(input, output));
  return true;
}

#endif /* D_SERIALIZER */