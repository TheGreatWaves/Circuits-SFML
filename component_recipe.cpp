#include "component_recipe.hpp"
#include "digital_serializer.hpp"

ComponentRecipe ComponentRecipe::construct_recipe(const Gate* gate)
{
  ComponentRecipe recipe{};

  // Set the recipe component name.
  recipe.set_component_name(gate->name);

  // Add sub components and mark dependency.
  for (const auto& [_, sub_component] : gate->subgates)
  {
    recipe.add_sub_component(sub_component->name);
  }

  // Add i/o pins.
  recipe.set_input_count(gate->input_pins.size());
  recipe.set_output_count(gate->output_pins.size());

  // Set serializable.
  if (gate->serialized) 
    recipe.set_serializable();
  else 
    recipe.set_not_serializable();

  return recipe;
}
