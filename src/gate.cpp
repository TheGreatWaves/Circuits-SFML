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
	components_up_to_date.push_back(false);
  auto key = subgate_count++;
	auto gate = board_instance->get_component(gate_name);
  subgates.push_back(gate->duplicate(board));

  for (auto& p : subgates[key]->input_pins)
  {
    p.parent = subgates[key].get();
		p.reset();
  }

  for (auto& p : subgates[key]->output_pins)
  {
		p.reset();
  }

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
    std::vector<Pin> exploring = to_explore;
    to_explore.clear();

    // Keep exploring until we reach a deadend or a parent component.
    std::size_t wires_explored = 0;

    while ( !exploring.empty() )
    {
      // Grab the pin we're interested in.
      auto pin = exploring.back();
      exploring.pop_back();

      // Add the pins it is connected to.
      for (auto& conn : pin.connections)
      {
        bool changed = false;
        PinState original_state = PinState::INACTIVE;

        if (conn->output != nullptr)
        {
          original_state = conn->output->state;          
        }

        conn->simulate();

        if (conn->output != nullptr)
        {
          changed = conn->output->state != original_state;          
        }

        if (changed 
        && conn->output->parent != nullptr 
        && was_visited.contains(conn->output->parent))
        {
          was_visited.erase(conn->output->parent);
        }

        if (conn->output != nullptr)
        {
          if (conn->output->parent == nullptr)
          {
            exploring.push_back(*conn->output);
          }
          else if (was_visited.count(conn->output->parent) == 0)
          {
             // Queue the component for next update.
            gates.push_back(conn->output->parent);
          }
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
    break; case GateType::CUSTOM: handle_custom_type(was_visited);
    break; default: log("Invalid type...?\n");
  }
}
