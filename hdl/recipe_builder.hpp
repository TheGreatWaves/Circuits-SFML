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

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../wire_info.hpp"

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
        std::cout << '\n';
        std::cout << meta();
    }

    [[nodiscard]] auto compile() const noexcept -> std::string
    {
        std::stringstream ss;

        comment(ss, "THIS DEFINITION FILE IS AUTO GENERATED. DO NOT EDIT MANUALLY.");
        ln(ss);

        // Adding dependencies.
        comment(ss, "DEPENDENCIES.");
        compile_dependencies(ss);
        ln(ss);

        // Create gate.
        comment(ss, "BEGINNING OF GATE DECLARATION.");
        writeln(ss, "create " + this->name);
        ln(ss);

        // Defining inputs.
        comment(ss, "GLOBAL INPUT PINS.");
        compile_input_pins(ss);
        ln(ss);

        // Defining inputs.
        comment(ss, "GLOBAL OUTPUT PINS.");
        compile_output_pins(ss);
        ln(ss);

        // Adding subgates.
        comment(ss, "SUBGATES");
        compile_subgates(ss);
        ln(ss);

        comment(ss, "LINKAGES");
        compile_linkage(ss);
        ln(ss);

        comment(ss, "DEFINITION COMPLETE.");
        writeln(ss, "save");
        ln(ss);

        writeln(ss, "// END OF FILE.");

        return ss.str();
    }

    /**
     * Set gate name.
     */
    auto set_gate_name(const std::string& name) noexcept -> void
    {
         this->name = name;   
    }

    /**
     * Generate the meta file.
     */
    [[nodiscard]] auto meta() const noexcept -> std::string
    {
        std::stringstream ss;

        comment(ss, "THIS META FILE IS AUTO GENERATED. DO NOT EDIT MANUALLY.");

        // Adding dependencies.
        ln(ss);
        comment(ss, "GATE NAME.");
        writeln(ss, name);

        // Defining inputs.
        ln(ss);
        comment(ss, "GLOBAL INPUT PINS.");
        writeln(ss, "INPUTS");
        for (std::size_t i = 0; i < input_pins.size(); i++)
        {
         writeln(ss, input_pins.at(i));
        }
        ln(ss);

        // Defining outputs.
        comment(ss, "GLOBAL OUTPUT PINS.");
        writeln(ss, "OUTPUTS");
        for (std::size_t i = 0; i < output_pins.size(); i++)
        {
         writeln(ss, output_pins.at(i));
        }
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
        input_pins[input_pins.size()] = name;
    }

    /**
     * The number of input pins.
     */
    [[nodiscard]] auto input_pin_size() const noexcept -> std::size_t
    {
        return input_pins.size();
    }

    /**
     * The number of output pins.
     */
    [[nodiscard]] auto output_pin_size() const noexcept -> std::size_t
    {
        return output_pins.size();
    }

    /**
     * Add output pin.
     */
    auto add_output_pin(const std::string& name) noexcept -> void
    {
        output_pins[output_pins.size()] = name;
    }

    /**
     * Add wire linkage.
     */
    auto add_wire_linkage(const std::size_t src, const std::size_t dest) noexcept -> void
    {
        wire_linkages.emplace_back(WireInfo{src, dest});
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

    auto compile_linkage(std::stringstream& ss) const noexcept -> void
    {
        for (const auto& linkage : wire_linkages)
        {
            ss << "wire " << linkage.src << ' ' << linkage.dest << '\n';
        }
    }

    auto compile_input_pins(std::stringstream& ss) const noexcept -> void
    {
        comment(ss, "INPUT PINS MAPPING.");
        for (std::size_t i = 0; i < input_pins.size(); i++)
        {
            const auto& pin = input_pins.at(i);
            ss << "// " << std::setw(2) << i << " " << pin << '\n';
        }
        ss << "input " << input_pins.size() << '\n';
    }

    auto compile_output_pins(std::stringstream& ss) const noexcept -> void
    {
        comment(ss, "OUTPUT PINS MAPPING.");
        for (std::size_t i = 0; i < output_pins.size(); i++)
        {
            const auto& pin = output_pins.at(i);
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
    std::string                        name;
    std::vector<std::string>           dependencies;
    std::map<std::size_t, std::string> input_pins;
    std::map<std::size_t, std::string> output_pins;
    std::vector<WireInfo>              wire_linkages;
};
} /* namespace hdl */

#endif /* HDL_RECIPE_BUILDER */
