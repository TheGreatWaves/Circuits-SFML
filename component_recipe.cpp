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

#include "component_recipe.hpp"
#include "gate.hpp"

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
