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

#include "gate.hpp"
#include "board.hpp"
#include "wire.hpp"

std::size_t Gate::add_subgate(std::string_view gate_name, Board* board)
{
  auto board_instance = (board == nullptr) ? Board::instance() : board;
  auto key = subgate_count++;
	auto gate = board_instance->get_component(gate_name);
  subgates.push_back(gate->duplicate(board));
  return key;
}

void Gate::handle_custom_type(std::unordered_set<Gate*> was_visited)
{
  was_visited.insert(this);

  if (serialized)
  {
    simulate_serialized();
    return;
  }

  std::vector<Pin> to_explore{ input_pins };
  std::vector<Gate*> gates{};
  std::size_t n {0};

  while( !to_explore.empty() )
  {
    std::vector<Pin> exploring = std::move(to_explore);

    // Keep exploring until we reach a deadend or a parent component.
    while ( !exploring.empty() )
    {
      // Grab the pin we're interested in.
      auto pin = exploring.back();
      exploring.pop_back();

      // Add the pins it is connected to.
      for (auto& conn : pin.connections)
      {
        if (conn->output == nullptr) continue;

        const PinState original_state = conn->output->state;          

        conn->simulate();

        const bool changed = conn->output->state != original_state;          

        if (changed 
        && conn->output->has_parent() 
        && was_visited.contains(conn->output->parent))
        {
          was_visited.erase(conn->output->parent);
        }

        if (!conn->output->has_parent())
        {
          exploring.push_back(*conn->output);
        }
        else if (!was_visited.contains(conn->output->parent))
        {
           // Queue the component for next update.
          gates.push_back(conn->output->parent);
        }
      }
    }
    for (const auto& gate : gates)
    {
      if (was_visited.count(gate) == 0)
      {
        was_visited.insert(gate);
        gate->simulate(was_visited);
        for (auto& output_pins : gate->output_pins)
        {
          to_explore.push_back(output_pins);
        }
      }
    }
    gates.clear();
  }
}

bool Gate::connect_pins(Pin* input, Pin* output)
{
  input->connections.push_back(std::make_shared<Wire>(input, output));
  wires.push_back(input->connections.back().get());
  return true;
}

void Gate::simulate(std::unordered_set<Gate*> was_visited) 
{
  // Simulate self.
  switch (type) 
  {
    break; case GateType::NAND: handle_nand();
    break; case GateType::DFF: handle_dff();
    break; case GateType::PC: handle_pc();
    break; case GateType::RAM_16K: handle_ram_16k();
    break; case GateType::MUX_16: handle_mux_16();
    break; case GateType::REGISTER: handle_register();
    break; case GateType::CUSTOM: handle_custom_type(was_visited);
    break; default: log("Invalid type...?\n");
  }
}

void Gate::handle_pc()
{
  static_cast<PC*>(this)->handle_pc_impl();
}

auto Gate::handle_register() -> void 
{
 static_cast<Register*>(this)->handle_register_impl(); 
}

void Gate::handle_ram_16k()
{
  static_cast<Ram16k*>(this)->handle_ram_16k_impl();
}

auto Gate::handle_mux_16() -> void 
{
  static_cast<Mux16*>(this)->handle_mux_16_impl();
}

std::unique_ptr<Gate> Gate::duplicate(Board* board)
{
  // Really ugly but it must be done.
  if (this->name == "pc") return std::make_unique<PC>();
  else if (this->name == "ram_16k") return std::make_unique<Ram16k>();
  else if (this->name == "mux_16") return std::make_unique<Mux16>();
  else if (this->name == "register") return std::make_unique<Register>();

  auto g = std::make_unique<Gate>(input_pins.size(), output_pins.size(), this->type, this->name, this->serialized);

  if (this->serialized)
  {
    // If it is serialized we don't actually need to do any wiring (no simulation).
    g->serialized_computation_ptr = this->serialized_computation_ptr;
    return g;
  }
  else
  {
    // Not serialized, which means that we need to simulate it. 
    // Add the subgates and copy the wiring.
    for (std::size_t i = 0; i < subgate_count; i++)
    {
      g->add_subgate(subgates.at(i)->name, board);
    }

    g->construct_wire(this->wire_construction_recipe);
   
    return g;
  }  
}
