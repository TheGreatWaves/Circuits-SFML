#include "gate.hpp"
#include "board.hpp"

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
