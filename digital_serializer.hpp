#pragma once
#ifndef D_SERIALIZER
#define D_SERIALIZER

#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <fstream>

#include "pin.hpp"
#include "utils.hpp"
#include "wire_info.hpp"

#define INPUT_PIN_LIMIT 100

extern std::size_t indent_level;

// List of built in types.
enum class GateType
{
  NAND,
  CUSTOM
};

struct Gate
{
  std::vector<Pin>                             input_pins;
  std::vector<Pin>                             output_pins;
  std::size_t                                  pin_count;
  GateType                                     type;
  std::size_t                                  subgate_count{};
  std::string                                  name;
  std::map<std::size_t, std::unique_ptr<Gate>> subgates;
  std::vector<Wire*> wires;
  WireConstructionInfo                         wire_construction_recipe;

  
  // Used for book keeping during simulate.
  std::vector<bool>                            components_up_to_date{};
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

  void print_truth_table()
  {
    if (!serialized)
    {
      log("Component not serialized, can't evaluate print table.\n");
      return;
    }
    std::size_t indicies = (2 << (input_pins.size() - 1));

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

    // Loop through all perms.
    for (std::size_t i = 0; i < indicies; i++)
    {
      auto output = serialized_computation.at(i);

      for (std::size_t j = 0; j < input_pins.size(); j++)
      {
        log(i >> (input_pins.size() - (1 + j)) & 1, ' ');
      }

      log("| ");
      for (std::size_t j = 0; j < output_pins.size(); j++)
      {
        log(output >> (output_pins.size() - (1 + j)) & 1, ' ');
      }
      newline();
    }
  }

  bool is_serialized()
  {
    return serialized;
  }

  void set_name(std::string_view new_name)
  {
    name = new_name;
  }
  
  const std::string& get_name() const
  {
    return name;
  }

  void serialize()
  {
    std::size_t indicies = (2 << (input_pins.size() - 1));
    for (std::size_t i = 0; i < indicies; i++)
    {
      apply_input(input_pins.size(), i);
      simulate();
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
  }

  void apply_output(int count, std::size_t mask)
  {
    std::size_t index = 0;
    while (count --> 0)
    {
      output_pins[index++].state = (((mask >> count ) & 1) == 1) ? PinState::ACTIVE : PinState::INACTIVE;
    }
  }

  void simulate()
  {
    // Simulate the input pins.
    switch (type)
    {
    break; case GateType::NAND: handle_nand();
    break; case GateType::CUSTOM: handle_custom_type();
    break; default: log("Invalid type...?\n");
    }

    // Simulate the output pins.
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


  void add_input_pin(int n = 1)
  {
    while (n --> 0)
    {
      pin_count++;
      input_pins.emplace_back();
    }
  }

  void add_output_pin(int n = 1)
  {
    while (n --> 0)
    {
      pin_count++;
      output_pins.emplace_back();
    }
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

  void clear_wires()
  {
    wires.clear();
  }

  void construct_wire(WireConstructionInfo& wire_info)
  {
    clear_wires();
    for (auto [src, dest] : wire_info)
    {
      if (!wire_pins(src, dest))
      {
        std::cerr << "Error: Failed constructing wire with " << src << " and " << dest << '\n';
      }
    }
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

    wire_construction_recipe.push_back({p1, p2});

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
  	return add_subgate(gate->name);
  }

  std::size_t add_subgate(std::string_view gate_name);

  std::unique_ptr<Gate> duplicate()
  {
    auto g = std::make_unique<Gate>(input_pins.size(), output_pins.size(), this->type, this->name, this->serialized);

    if (this->serialized)
    {
      // If it is serialized we don't actually need to do any wiring (no simulation).
      g->serialized_computation = this->serialized_computation;
      return g;
    }
    else
    {
      // Not serialized, which means that we need to simulate it. 
      // Add the subgates and copy the wiring.
      for (std::size_t i = 0; i < subgate_count; i++)
      {
        g->add_subgate(subgates.at(i)->name);
      }

      g->construct_wire(this->wire_construction_recipe);
     
      return g;
    }
  }

  void handle_custom_type(std::vector<bool>* visited = nullptr, 
    std::map<std::size_t, std::unique_ptr<Gate>>* components = nullptr
  );

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


  void input_info()
  {
    log("Input Info:\n");
    for (std::size_t count = 0; count < input_pins.size(); count++)
		{
			log("pin[", count, "] ", input_pins[count].state == PinState::ACTIVE ? 1 : 0, "\n");
		}
  }

  void output_info()
  {
    log("output Info:\n");
    for (std::size_t count = 0; count < output_pins.size(); count++)
		{
			log("pin[", count + INPUT_PIN_LIMIT, "] ", output_pins[count].state == PinState::ACTIVE ? 1 : 0, "\n");
		}
  }


  void subgates_info()
  {
    auto count = input_pins.size();
    auto output_count = output_pins.size();
    for (const auto& subgate : subgates)
    {
      newline();
      log(BLOCK, " Subgate[", subgate.first, " : ", subgate.second->name, "] ", BLOCK, "\n");
  		for (const auto& pin : subgate.second->input_pins)
  		{
			  log("    pin[", count, "] ", pin.state == PinState::ACTIVE ? 1 : 0, "\n");
  			count++;
  		}
      log("Exposed output pins:\n");
  		for (const auto& pin : subgate.second->output_pins)
  		{
  			log("    pin[", output_count + INPUT_PIN_LIMIT, "] ", pin.state == PinState::ACTIVE ? 1 : 0, "\n");
  			output_count++;
  		}
    }  
  }

  void subgates_brief()
  {
    auto count = input_pins.size();
    auto output_count = output_pins.size();
    for (const auto& subgate : subgates)
    {
      newline();
      log(BLOCK, " Subgate[", subgate.first, " : ", subgate.second->name, "] ", BLOCK, "\n");
  		for (const auto& pin : subgate.second->input_pins)
  		{
			  log("> ", count, '\n');
  			count++;
  		}
      log("Exposed output pins:\n");
  		for (const auto& pin : subgate.second->output_pins)
  		{
			  log("> ", output_count + INPUT_PIN_LIMIT, '\n');
  			output_count++;
  		}
    }  
  }


  void input_pin_address_info()
  {
    log("Input pins address:\n");
		for (const auto& pin : input_pins)
		{
		  log("> ", &pin, '\n');
		}
    newline();
  }


  void wire_info()
  {
    log("Wire count: ", wires.size(), '\n');
    for (auto [src, dest] : wire_construction_recipe)
    {
      std::cout << src << " <---> " << dest << '\n';
    }  }
  
  // Prints information about the current gate.
  void info()
  {
    log(BLOCK, " Gate ", name, BLOCK, '\n');
    log("Is serialized: ", (serialized ? "yes" : "no"), '\n');
    wire_info();
    input_pin_address_info();

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
			log("pin[", output_count + INPUT_PIN_LIMIT, "] ", pin.state == PinState::ACTIVE ? 1 : 0, "\n");
			output_count++;
		}

    subgates_info();
  }
};

#include "wire.hpp"

inline void Gate::handle_custom_type(std::vector<bool>* visited, 
    std::map<std::size_t, std::unique_ptr<Gate>>* components)
{
  if (serialized)
  {
    simulate_serialized();
    return;
  }

  for (auto& subgate : subgates)
  {
    for (auto& wire : wires)
    {
      wire->simulate();
    }
    // std::cout << "    HANDLING SUBGATE: " << subgate.first << '\n';
    // subgate.second->input_info();
    subgate.second->simulate();
    // std::cout << "    HANDLING SUBGATE: DONE " << subgate.first << '\n';
    // subgate.second->info();
  }

  for (auto& wire : wires)
  {
    wire->simulate();
  }
}

inline bool Gate::connect_pins(Pin* input, Pin* output)
{
  input->connections.push_back(std::make_shared<Wire>(input, output));
  wires.push_back(input->connections.back().get());
  return true;
}

#endif /* D_SERIALIZER */