#include "gate.hpp"
#include "board.hpp"
#include "wire.hpp"

std::size_t Gate::add_subgate(std::string_view gate_name)
{
	components_up_to_date.push_back(false);
  auto key = subgate_count++;
	auto gate = Board::instance()->get_component(gate_name);
  subgates[key] = gate->duplicate();

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

  /**
   * Basic BFS.
   */
  std::vector<Pin> to_explore{ input_pins };
  std::vector<Gate*> gates{};
  std::size_t n {0};

  // std::cout << "\n\n\nHandling: [" << this->name << "] " << this << "\n";
  
  while( !to_explore.empty() )
  {
  //   std::cout << "Number of pins to explore: " << to_explore.size() << '\n';
  //   std::cout << "Input State: 0b";

    for (auto& p : to_explore)
    {
  //     std::cout << p.is_active();
    }
  //   std::cout << '\n';
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

      for (auto conn : pin.connections)
      {
        conn->simulate();
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
  //   std::cout << "\n=== Adding gates ===\n";
    for (auto gate : gates)
    {
      if (was_visited.count(gate) == 0)
      {
  //       std::cout << "|> " << gate->name << '\n';
        was_visited.insert(gate);

        gate->simulate(was_visited);

  //       std::cout << "|Output pins: 0b";
        for (auto& output_pins : gate->output_pins)
        {
  //         std::cout << output_pins.is_active();
          to_explore.push_back(output_pins);
        }
  //       std::cout << '\n';
      }
    }
  //   std::cout << '\n';
    gates.clear();
  }
  // std::cout << "Finished handling: [" << this->name << "] " << this << "\n";
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
