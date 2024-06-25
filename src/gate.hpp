/** 
 * MIT License
 * 
 * Copyright (c) 2023 Ochawin A.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#ifndef GATE_H
#define GATE_H

#include <memory>
#include <vector>
#include <unordered_set>

#include "common.hpp"
#include "pin.hpp"
#include "utils.hpp"
#include "wire_info.hpp"

/**
 * Different type of gates. At the moment we only have one built-in type.
 */
enum class GateType
{
  NAND,
  DFF,
  PC,
  RAM_16K,
  ROM_32K,
  REGISTER,
  MUX_16,
  CUSTOM
};

class Board;


/**
 * A Gate represents a component board. Each component board consists of I/O ports,
 * subgates and wires. The gate also contains its wire construction recipe for the
 * sake of self replication.
 */
struct Gate
{
  /**
   * Gate information.
   */
  GateType                                     type;
  std::size_t                                  subgate_count{};
  std::string                                  name{};
  WireConstructionInfo                         wire_construction_recipe{};
  std::size_t                                  pin_count{};
  std::vector<std::unique_ptr<Gate>>           subgates{};
  bool                                         serialized{};
  std::vector<std::size_t>                     serialized_computation{};
  std::vector<std::size_t>*                    serialized_computation_ptr{ nullptr };

  /**
   * Input/Output ports and wires.
   *
   * NOTE: This information can probably be pulled out into a struct. The struct
   * then can hold a pointer to an 'image/profile' of the gate (the Gate class).
   * That will give it sufficient amount of information that is required for simulation.
   * Then when we want to duplicate a chip, we can just duplicate the struct. This will
   * help us avoid duplicating redundant information which only needs to be constructed once
   * like wire_construction_recipe. There is no point for every single chip to be holding
   * the same information
   */
  std::vector<Pin>                             input_pins{};
  std::vector<Pin>                             output_pins{};
  std::vector<Wire*> wires;
  

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

  auto print_truth_table() -> void
  {
    if (!serialized)
    {
      log("Component not serialized, can't evaluate print table.");
      return;
    }
    std::size_t indicies = (2 << (input_pins.size() - 1));

    for (std::size_t n = 0; n < input_pins.size(); n++)
    {
      print(n, ' ');
    }

    print("| ");

    for (std::size_t n = 0; n < output_pins.size(); n++)
    {
      print(n, ' ');
    }

    newline();
    for (std::size_t n = 0; n < (input_pins.size() + output_pins.size()); n++)
    {
      print("==");
    }
    print("=");
    newline();

    // Loop through all perms.
    for (std::size_t i = 0; i < indicies; i++)
    {
      auto output = (*serialized_computation_ptr).at(i);

      for (std::size_t j = 0; j < input_pins.size(); j++)
      {
        print(i >> (input_pins.size() - (1 + j)) & 1, ' ');
      }

      print("| ");
      for (std::size_t j = 0; j < output_pins.size(); j++)
      {
        print(output >> (output_pins.size() - (1 + j)) & 1, ' ');
      }
      newline();
    }
  }

  auto is_serialized() const -> bool
  {
    return serialized;
  }

  auto set_name(std::string_view new_name) -> void
  {
    name = new_name;
  }
  
  const std::string& get_name() const
  {
    return name;
  }

  auto serialize() -> void
  {
    uint64_t indicies = (2ULL << (static_cast<uint64_t>(input_pins.size()) - 1));

    for (uint64_t i = 0; i < indicies; i++)
    {
      apply_input(input_pins.size(), static_cast<std::size_t>(i));
      simulate();
      serialized_computation.push_back(serialize_output());
    }
    this->serialized = true;
    this->serialized_computation_ptr = &this->serialized_computation;
  }

  inline auto serialize_output() -> std::size_t
  {
    return pinvec_to_uint(output_pins, 0, output_pins.size());
  }

  inline auto serialize_input() -> std::size_t
  {
    return pinvec_to_uint(input_pins, 0, input_pins.size());
  }

  auto simulate_serialized() -> void
  {
    // Get serialized entry of the input.
    auto serialized_input = serialize_input();

    // Retrieve the serialized output entry using the serialized input.
    auto serialized_output = (*serialized_computation_ptr)[serialized_input];

    apply_output(static_cast<int>(output_pins.size()), serialized_output);
  }

  auto apply_input(int count, std::size_t mask) -> void
  {
    std::size_t index = 0;
    while (count --> 0)
    {
      input_pins[index++].state = (((mask >> count ) & 1) == 1) ? PinState::ACTIVE : PinState::INACTIVE;
    }
  }

  auto apply_output(int count, std::size_t mask) -> void
  {
    std::size_t index = 0;
    while (count --> 0)
    {
      output_pins[index].state = (((mask >> count ) & 1) == 1) ? PinState::ACTIVE : PinState::INACTIVE;
      index++;
    }
  }

  void simulate(std::unordered_set<Gate*> was_visited = {});

  auto reset() -> void
  {
		log("Resetting output pins...\n");
    for (auto& output_pin : output_pins)
    {
      output_pin.reset();
    }
  }


  auto add_input_pin(int n = 1) -> void
  {
    // We update all the pins which are above this number.
    auto old_input_count = input_pins.size();

    // Update output pins recipe for coherency.
    for (auto& [a, b] : wire_construction_recipe)
    {
      if (a >= old_input_count && a < INPUT_PIN_LIMIT)
      {
        a += n;
      }
      if (b >= old_input_count && b < INPUT_PIN_LIMIT)
      {
        b += n;
      }
    }

    while (n --> 0)
    {
      pin_count++;
      input_pins.emplace_back();
    }
  }

  auto add_output_pin(int n = 1) -> void
  {
    // We update all the pins which are above this number.
    auto old_output_count = output_pins.size() + INPUT_PIN_LIMIT;

    // Update output pins recipe for coherency.
    for (auto& [a, b] : wire_construction_recipe)
    {
      if (a >= old_output_count)
      {
        a += n;
      }
      if (b >= old_output_count)
      {
        b += n;
      }
    }

    while (n --> 0)
    {
      pin_count++;
      output_pins.emplace_back();
    }

  }

  auto get_pin(std::size_t pin) -> Pin*
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
          auto s = subgate->input_pins.size();
          range += s;
          if ( range > pin )
          {
            range -= s;
            return &subgate->input_pins.at(pin - range);
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
          auto s = subgate->output_pins.size();
          range += s;
          if ( range > pin )
          {
            range -= s;
            return &subgate->output_pins.at(pin - range);
          }
        }
      }
    }

    return nullptr;
  }

  auto clear_wires() -> void
  {
    wires.clear();
  }

  auto construct_wire(WireConstructionInfo& wire_info) -> void
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

  auto wire_pins(std::size_t p1, std::size_t p2) -> bool
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

  auto has_pin(std::size_t input_id) -> bool
  {
    return input_pins.size() > input_id;
  }

  auto get_pin_state(std::size_t input_id) -> PinState
  {
    return input_pins.at(input_id).state;
  }

  auto toggle_pin(std::size_t input_id) -> bool
  {
    if (input_pins.size() < input_id) return false;
    input_pins[input_id].flip();
    return true;
  }

  auto add_subgate(Gate* gate, Board* board = nullptr) -> std::size_t
  {
  	return add_subgate(gate->name, board);
  }

  std::size_t add_subgate(std::string_view gate_name, Board* board = nullptr);

  std::unique_ptr<Gate> duplicate(Board* board = nullptr);

  void handle_custom_type(std::unordered_set<Gate*> was_visited);

  /**
   * Built-in type handlers.
   * TODO: Maybe abstract this out later.
   */
  auto handle_nand() -> void
  {
    output_pins[0].state = (!(input_pins[0].is_active() && input_pins[1].is_active()))
                         ? PinState::ACTIVE
                         : PinState::INACTIVE;
  }

  auto handle_dff() -> void
  {
    if (input_pins[1].is_active())
    {
      output_pins[0].state = input_pins[0].get_state();
    }
  }

  void handle_pc();

  void handle_ram_16k();

  void handle_rom_32k();

  auto handle_mux_16() -> void;

  auto handle_register() -> void;

  auto input_info() -> void
  {
    log("Input Info:\n");
    for (std::size_t count = 0; count < input_pins.size(); count++)
		{
			log("pin[", count, "] ", input_pins[count].state == PinState::ACTIVE ? 1 : 0, "\n");
		}
  }

  auto output_info() -> void
  {
    log("output Info:\n");
    for (std::size_t count = 0; count < output_pins.size(); count++)
		{
			log("pin[", count + INPUT_PIN_LIMIT, "] ", output_pins[count].state == PinState::ACTIVE ? 1 : 0, "\n");
		}
  }


  auto subgates_info() -> void
  {
    auto count = input_pins.size();
    auto output_count = output_pins.size();
    for (auto i=0; i < subgates.size(); i++)
    {
      const auto& subgate = subgates.at(i);
      newline();
      log(BLOCK, " Subgate[", i, " : ", subgate->name, "] ", BLOCK, "\n");
  		for (const auto& pin : subgate->input_pins)
  		{
			  log("    pin[", count, "] ", pin.state == PinState::ACTIVE ? 1 : 0, "\n");
  			count++;
  		}
      log("Exposed output pins:\n");
  		for (const auto& pin : subgate->output_pins)
  		{
  			log("    pin[", output_count + INPUT_PIN_LIMIT, "] ", pin.state == PinState::ACTIVE ? 1 : 0, "\n");
  			output_count++;
  		}
    }  
  }

  auto subgates_brief() -> void
  {
    auto count = input_pins.size();
    auto output_count = output_pins.size();
    for (auto i=0; i < subgates.size(); i++)
    {
      const auto& subgate = subgates.at(i);
      newline();
      log(BLOCK, " Subgate[", i, " : ", subgate->name, "] ", BLOCK, "\n");
  		for (const auto& pin : subgate->input_pins)
  		{
			  log("> ", count, '\n');
  			count++;
  		}
      log("Exposed output pins:\n");
  		for (const auto& pin : subgate->output_pins)
  		{
			  log("> ", output_count + INPUT_PIN_LIMIT, '\n');
  			output_count++;
  		}
    }  
  }


  auto input_pin_address_info() -> void
  {
    log("Input pins address:\n");
		for (const auto& pin : input_pins)
		{
		  log("> ", &pin, '\n');
		}
    newline();
  }


  auto wire_info() -> void
  {
    log("Wire count: ", wires.size(), '\n');
    for (auto [src, dest] : wire_construction_recipe)
    {
      std::cout << src << " <---> " << dest << '\n';
    }  
  }
  
  // Prints information about the current gate.
  auto info() -> void
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

#endif /* GATE_H */
