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
#ifndef HDL_META
#define HDL_META

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "../../common.hpp"
#include "../core/raw_parser.hpp"
#include "recipe_builder.hpp"
#include "../core/token.hpp"
#include "../core/trie.hpp"

namespace hdl
{

/**
 * The Meta struct holds relevant data needed by the .hdl parser.
 */
struct Meta
{
    struct PinEntry
    {
        std::string pin_name;
        std::size_t pin_number;

        PinEntry(const std::string& name, std::size_t number)
            : pin_name{name}
            , pin_number{number}
        {
        }
    };


    [[nodiscard]] auto get_pin(std::string_view name) const noexcept -> const std::optional<PinEntry>
    {
        auto pin_name = std::string(name);
        for (const auto& pin_entry : input_pins)
        {
            if (trie.match(pin_entry.pin_name, pin_name))
            {
                return pin_entry;
            }
        }

        for (const auto& pin_entry : output_pins)
        {
            if (trie.match(pin_entry.pin_name, pin_name))
            {
                return pin_entry;
            }
        }

        return {};
    }

    [[nodiscard]] auto get_bus(std::string_view name) const noexcept -> const std::optional<BusEntry>
    {
        auto bus_name = std::string(name);
        for (const auto& bus_entry : bus)
        {
            if (trie.match(bus_entry.bus_name, bus_name))
            {
                return bus_entry;
            }
        }

        return {};
    }

    [[nodiscard]] static inline auto get_meta(std::string_view component_name) -> std::unique_ptr<const Meta>;

    auto add_input_pin(std::string_view pin_name) -> void
    {
        this->input_pins.emplace_back(std::string(pin_name), input_pins.size());
        this->trie.insert(std::string(pin_name));
        this->input_count++;
    }

    auto add_input_pins(std::vector<std::string_view> pin_names) -> void
    {
        for (auto pin_name: pin_names)
        {   
            add_input_pin(pin_name);
        }
    }

    auto add_output_pins(std::vector<std::string_view> pin_names) -> void
    {
        for (auto pin_name: pin_names)
        {   
            add_output_pin(pin_name);
        }
    }

    auto add_output_pin(std::string_view pin_name) -> void
    {
        this->output_pins.emplace_back(std::string(pin_name), MAX_INPUT_PINS + output_pins.size());
        this->trie.insert(std::string(pin_name));
        this->output_count++;
    }

    auto add_input_bus(std::string_view bus_name, std::size_t size)
    {
        int32_t s = static_cast<int32_t>(size);
        this->trie.insert(std::string(bus_name));
        this->bus.emplace_back(std::string(bus_name), this->input_count, size);

        while (s --> 0)
        {
            std::stringstream ss;
            ss << bus_name << '[' << s << ']';
            add_input_pin(ss.str());
        }
    }

    auto add_output_bus(std::string_view bus_name, std::size_t size) -> void
    {
        int32_t s = static_cast<int32_t>(size);
        this->trie.insert(std::string(bus_name));
        this->bus.emplace_back(std::string(bus_name), MAX_INPUT_PINS + this->output_count, size);

        while (s --> 0)
        {
            std::stringstream ss;
            ss << bus_name << '[' << s << ']';
            add_output_pin(ss.str());
        }
    }

    auto set_name(std::string_view name) -> void
    {
        this->name = name;
        this->trie.insert(std::string(name));
    }

    std::string           name{};
    std::size_t           input_count{};
    std::size_t           output_count{};
    std::vector<PinEntry> input_pins{};
    std::vector<PinEntry> output_pins{};

    std::vector<BusEntry> bus{};
    Trie                  trie;
};

[[nodiscard]] inline auto Meta::get_meta(std::string_view component_name) -> std::unique_ptr<const Meta>
{
    auto meta = std::make_unique<Meta>();

    if (component_name == "nand")
    {
        meta->set_name("nand");
        meta->add_input_pins({"a", "b"});
        meta->add_output_pin("out");
        return meta;
    }
    else if (component_name == "dff")
    {
        meta->set_name("dff");
        meta->add_input_pins({"in", "clock"});
        meta->add_output_pin("out");
        return meta;
    }
    else if (component_name == "pc")
    {
        meta->set_name("pc");
        meta->add_input_bus("in", 16);
        meta->add_input_pins({"load", "inc", "reset", "clock"});
        meta->add_output_bus("out", 16);
        return meta;
    }
    else if (component_name == "ram_16k")
    {
        meta->set_name("ram_16k");

        meta->add_input_bus("in", 16);
        meta->add_input_bus("address", 14);
        meta->add_input_pins({"load", "clock"});

        meta->add_output_bus("out", 16);

        return meta;
    }

    try
    {
        auto parser = RawParser(SCRIPTS_DIR + "/" + std::string(component_name) + ".meta");
        static_cast<void>(parser.advance_token());

        // Parse the name of the component
        parser.consume_token(RawTokenType::Identifier, "Expected identifier.");
        meta->name = parser.get_previous().lexeme;

        
        parser.consume_token(RawTokenType::Identifier, "Expected identifier.");
        const auto prev = parser.get_previous().lexeme;

        if (prev == "BUSES")
        {
            parser.consume_token(RawTokenType::Number, "Expected BUSES count (number), found " + parser.get_current().lexeme);
            const int bus_count = std::stoi(parser.get_previous().lexeme);

            for (int i = 0; i < bus_count; i++)
            {
                // Parse BUS name.
                parser.consume_token(RawTokenType::Identifier, "Expected identifier.");
                const auto bus_name = parser.get_previous().lexeme;

                // Parse BUS start.
                parser.consume_token(RawTokenType::Number, "Expected number.");
                const auto bus_start = std::stoi(parser.get_previous().lexeme);

                // Parse BUS size.
                parser.consume_token(RawTokenType::Number, "Expected number.");
                const auto bus_size = std::stoi(parser.get_previous().lexeme);

                // Add the bus name entry.
                meta->trie.insert(bus_name);

                // Add the bus entry.
                meta->bus.emplace_back(bus_name, bus_start, bus_size);
            }

            // Move onto 'INPUTS'
            parser.consume_token(RawTokenType::Identifier, "Expected identifier.");
        }
        
        // Parse the the count of inputs.
        if (auto lexeme = parser.get_previous().lexeme; lexeme != "INPUTS")
        {
            parser.report_custom_error("Expected 'INPUTS', found " + lexeme);
        }

        parser.consume_token(RawTokenType::Number, "Expected INPUTS count (number), found " + parser.get_current().lexeme);
        meta->input_count = std::stoi(parser.get_previous().lexeme);
        for (std::size_t i = 0; i < meta->input_count; i++)
        {
            parser.consume_token(RawTokenType::Identifier, "Expected identifier, found '" + parser.get_current().lexeme + "'.");
            auto lexeme = parser.get_previous().lexeme;

            if (parser.match_token(RawTokenType::LSqaure))
            {
                parser.consume_token(RawTokenType::Number, "Expected number, found" + parser.get_current().lexeme);
                auto count = parser.get_previous().lexeme;
                parser.consume_token(RawTokenType::RSquare, "Expected ']', found " + parser.get_current().lexeme);              
                lexeme = lexeme + "[" + count + "]";
            }

            meta->input_pins.emplace_back(lexeme, i);
            meta->trie.insert(lexeme);
        }

        // Parse the the count of outputs.
        parser.consume_token(RawTokenType::Identifier, "Expected identifier.");
        if (auto lexeme = parser.get_previous().lexeme; lexeme != "OUTPUTS") 
        {
                parser.report_custom_error("Expected 'outputS', found " + lexeme);
        }

        parser.consume_token(RawTokenType::Number, "Expected outputs count (number), found " + parser.get_current().lexeme);
        meta->output_count = std::stoi(parser.get_previous().lexeme);
        for (std::size_t i = 0; i < meta->output_count; i++)
        {
            parser.consume_token(RawTokenType::Identifier, "Expected identifier, found '" + parser.get_current().lexeme + "'.");
            auto lexeme = parser.get_previous().lexeme;

            if (parser.match_token(RawTokenType::LSqaure))
            {
                parser.consume_token(RawTokenType::Number, "Expected number, found" + parser.get_current().lexeme);
                auto count = parser.get_previous().lexeme;
                parser.consume_token(RawTokenType::RSquare, "Expected ']', found " + parser.get_current().lexeme);              
                lexeme = lexeme + "[" + count + "]";
            }

            meta->output_pins.emplace_back(lexeme, i + MAX_INPUT_PINS);
            meta->trie.insert(lexeme);
        }

        if (parser.get_error())
        {
            throw std::runtime_error("Parser error occured.");
        }

        return std::move(meta);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return nullptr;
}

} /* namespace hdl */

#endif /* HDL_META */
