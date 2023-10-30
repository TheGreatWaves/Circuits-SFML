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
#ifndef HDL_RECIPE_BUILDER
#define HDL_RECIPE_BUILDER

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

constexpr std::size_t MAX_INPUT_PINS = 100;

namespace hdl
{

/**
 * The RecipeBuilder is an interface which makes creating components more
 * user friendly.
 */
class RecipeBuilder
{
  public:
    auto build() const noexcept -> void
    {
        // Debug
        std::cout << compile();
    }

    [[nodiscard]] auto compile() const noexcept -> std::string
    {
        std::stringstream ss;

        writeln(ss, "// THIS DEFINITION FILE IS AUTO GENERATED. DO NOT EDIT MANUALLY.");

        // Adding dependencies.
        ln(ss);
        writeln(ss, "// DEPENDENCIES.");
        compile_dependencies(ss);

        // Defining inputs.
        ln(ss);
        writeln(ss, "// GLOBAL INPUT PINS.");
        compile_input_pins(ss);
        ln(ss);

        // Defining inputs.
        writeln(ss, "// GLOBAL OUTPUT PINS.");
        compile_output_pins(ss);
        ln(ss);

        // Adding subgates.
        comment(ss, "SUBGATES");
        compile_subgates(ss);
        ln(ss);

        writeln(ss, "// END OF FILE.");

        return ss.str();
    }

    /**
     * Add to dependency list.
     */
    auto add_dependency(const std::string& dependency) noexcept -> void
    {
        dependencies.emplace_back(dependency);
    }

    /**
     * Add input pin.
     */
    auto add_input_pin(const std::string& name) noexcept -> void
    {
        input_pins.emplace_back(name);
    }

    /**
     * Add output pin.
     */
    auto add_output_pin(const std::string& name) noexcept -> void
    {
        output_pins.emplace_back(name);
    }

    /**
     * PRIVATE METHODS.
     */
  private:
    auto compile_dependencies(std::stringstream& ss) const noexcept -> void
    {
        for (const auto& dependency : dependencies)
        {
            ss << "need " << dependency << '\n';
        }
    }

    auto compile_subgates(std::stringstream& ss) const noexcept -> void
    {
        for (const auto& dependency : dependencies)
        {
            ss << "add " << dependency << '\n';
        }
    }

    auto compile_input_pins(std::stringstream& ss) const noexcept -> void
    {
     comment(ss, "INPUT PINS MAPPING.");
     for (std::size_t i = 0; i < input_pins.size(); i++)
     {
       const auto& pin = input_pins[i];
       ss << "// " << std::setw(2) << i << " " << pin << '\n';
     }
     ss << "input " << input_pins.size() << '\n';
    }

    auto compile_output_pins(std::stringstream& ss) const noexcept -> void
    {
     comment(ss, "OUTPUT PINS MAPPING.");
     for (std::size_t i = 0; i < output_pins.size(); i++)
     {
       const auto& pin = output_pins[i];
       ss << "// " << std::setw(3) << (i + MAX_INPUT_PINS) << " " << pin << '\n';
     }
     ss << "output " << output_pins.size() << '\n';
    }

    auto writeln(std::stringstream& ss, const std::string& str) const noexcept -> void
    {
        ss << str << '\n';
    }

    auto comment(std::stringstream& ss, const std::string& str) const noexcept -> void
    {
        ss << "// " << str << '\n';
    }

    auto ln(std::stringstream& ss) const noexcept -> void
    {
        ss << '\n';
    }

  private:
    std::vector<std::string> dependencies;
    std::vector<std::string> input_pins;
    std::vector<std::string> output_pins;
};
} /* namespace hdl */

#endif /* HDL_RECIPE_BUILDER */
