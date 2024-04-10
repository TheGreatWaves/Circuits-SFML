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
#include <cmath>
#ifndef TESTER_H
#define TESTER_H

#include <map>
#include <string>
#include <algorithm>

#include "../../common.hpp"
#include "../../board.hpp"
#include "../core/parser_base.hpp"
#include "../hdl/meta.hpp"
#include "token_test.hpp"

namespace test
{

namespace _priv
{
    inline std::size_t pow2(std::size_t n) noexcept
    {
        return (1 << n);
    }
}

struct ChipInfo 
{
  Gate*                            gate;  
  std::unique_ptr<const hdl::Meta> meta;
};

struct Variable
{
 ChipInfo*                  chip_info;       
 Gate*                      chip;
 std::map<std::string, int> values;
};

enum class ValueType
{
  Number,
  Member,  
};

struct Value
{
    ValueType type;
    std::string value;
    std::string member;
};

enum class ConditionType
{
 IS,
 NOT,     
};

struct Condition
{
    Value a;
    Value b;
    ConditionType type;  
};

/**
 * An interpreter for the test code.
 */
class Tester : public BaseParser<TestTokenTypeScanner, TestTokenType>
{
  public:
    /**
     * Constructor with file path of test source code.
     */
    [[nodiscard]] explicit Tester(const std::string& file_path)
        : BaseParser<TestTokenTypeScanner, TestTokenType>(file_path)
        , board(false)
    {
    }

    /**
     * Default Ctor prohibted.
     */
    constexpr Tester() = delete;

    /**
     * Parse the source code and build the RecipeBuilder.
     */
    [[nodiscard]] auto parse() noexcept -> bool
    {
        advance();

        while (!match(TestTokenType::EndOfFile))
        {
            declaration();
        }

        return !this->has_error;
    }

    /**
     * Parsing functions.
     */
  private:
    auto declaration() noexcept -> void
    {
        if (match(TestTokenType::Load))
        {
            LOAD_statement();
        }
        else if (match(TestTokenType::Test))
        {
            TEST_statement();
        }

        if (this->panic)
        {
            restabilize();
        }
    }

    auto LOAD_impl(const std::string& chip_name) noexcept -> void
    {
        const auto path = GATE_RECIPE_DIRECTORY + chip_name + GATE_EXTENSION;
        log("Loading: " + path);

        if (!board.load_file(path))
        {
            report_error("'" + chip_name + ".gate' failed to load.");
            return;
        }

        auto meta = hdl::Meta::get_meta(chip_name);
        if (meta == nullptr)
        {
            report_error("Chip '" + chip_name + "' meta data not found.");
            return;
        }

        chip_images[chip_name] = { board.get_component(chip_name), std::move(meta) };
    }

    auto LOAD_statement() noexcept -> void
    {
        log("Parsing LOAD statement.");

        consume(TestTokenType::Identifier, "Expected chip name.");
        const auto chip_name = previous.lexeme;

        log("Loading chip: " + chip_name);

        if (!this->has_error)
            LOAD_impl(chip_name);

        expect_semicolon("Expected ';' at the end of LOAD statement.");


        log("Finished parsing LOAD statement.");
    }

    auto parse_quote() noexcept -> std::string
    {
        std::stringstream val{};

        while (!match(TestTokenType::Quote))
        {
            if (current.type == TestTokenType::LBrace)
            {
                const auto test_name = val.str();
                report_error("Invalid test name: " + test_name);
                return "";
            }

            advance();
            val << previous.lexeme << " ";

        }

        return val.str();
    }

    auto expect_semicolon(const std::string& message) noexcept -> void
    {
        consume(TestTokenType::Semicolon, message);
    }

    auto VAR_impl(const std::string& varname, const std::string& type) noexcept -> void
    {
        if (chip_images.count(type) == 0)
        {
            report_error("Chip type '" + type + "' not found.");
            return;
        }

        ChipInfo* image = &chip_images.at(type);

        std::map<std::string, int> values{};
        auto add_value = [&](const auto& pin) { values[pin.pin_name] = 0; };

        std::for_each(image->meta->input_pins.begin(), 
                      image->meta->input_pins.end(), 
                      add_value);

        std::for_each(image->meta->output_pins.begin(), 
                      image->meta->output_pins.end(), 
                      add_value);

        std::for_each(image->meta->bus.begin(), 
                      image->meta->bus.end(), 
                      [&](const auto& bus) { values[bus.bus_name] = 0; });

        const auto key = board.context().second->add_subgate(image->gate, &board);
        auto chip = board.context().second->subgates[key].get();

        variables[varname] = { image, chip, std::move(values) };
    }

    auto VAR_statement() noexcept -> void
    {
        log("Parsing VAR statement.");
        consume(TestTokenType::Identifier, "Expected variable name.");
        const auto varname = previous.lexeme;

        consume(TestTokenType::colon, "Expected ':' after variable name.");

        consume(TestTokenType::Identifier, "Expected variable type.");
        const auto vartype = previous.lexeme;

        log("Varname: " + varname);
        log("Type: " + vartype);

        expect_semicolon("Expected ';' at the end of VAR statement.");

        VAR_impl(varname, vartype);
        
        log("Finished parsing VAR statement.");
    }

    auto EVAL_impl() noexcept -> void
    {
        log("RUNNING EVAL!!!");
        for (auto& [_, variable] : variables)
        {
            int sim_count = 1;
            while (sim_count --> 0)
            {
                variable.chip->simulate();
            }

            for (const auto& [name, number] : variable.chip_info->meta->output_pins)
            {
                const auto state = (variable.chip->get_pin(number)->is_active()) ? 1 : 0;
                variable.values[name] = state;
                log("Setting " + name + " to " + std::to_string(state));
            }
            for (const auto& [name, start, size] : variable.chip_info->meta->bus)
            {
                // We only want outputs.
                if (start < MAX_INPUT_PINS) 
                    continue;

                std::size_t so = 0;
                for (std::size_t offset = 0; offset < size; offset++)
                {
                  so <<= 1;
                  so |= (variable.chip->output_pins.at(start + (size-1) - offset - MAX_INPUT_PINS).is_active() ? 1 : 0);
                }
                variable.values[name] = so;
                log("Setting bus " + name + " to " + std::to_string(so));
            }
        }
    }

    auto EVAL_statement() noexcept -> void
    {
        // Simulate...
        EVAL_impl();
        expect_semicolon("Expected semicolon for EVAL statement.");
    }

    // TODO: Make this return variable information.
    auto parse_variable() noexcept -> Value
    {
        log("Parsing VARIABLE.");
        if (match(TestTokenType::Number))
        {
            const auto number = previous.lexeme;
            log("Returning numeric constant: " + number);
            return { .type=ValueType::Number, .value=number };
        }
        else if (match(TestTokenType::Identifier)) 
        {
            const auto varname = previous.lexeme;

            consume(TestTokenType::Dot, "Expected '.' after variable name to access member.");

            consume(TestTokenType::Identifier, "Expected variable member name.");
            auto member = previous.lexeme;

            if (match(TestTokenType::LSqaure))
            {
                consume(TestTokenType::Number, "Expected bus index, found '" + current.lexeme + "'.");
                const auto bus_index = previous.lexeme;

                consume(TestTokenType::RSquare, "Expected ']', found '" + current.lexeme + "'.");

                member = member + "[" + bus_index + "]";
            }

            log("Varname: " + varname);
            log("Member: " + member);
            log("Finished parsing VARIABLE.");
        
            return { .type=ValueType::Member, .value=varname, .member=member };
        }
        else
        {
          report_error("Expected variable.");
          return {};
        }
    }

    auto get_value(const Value& value) noexcept -> int 
    {
        switch (value.type)   
        {
            break; case ValueType::Number:
            {
                return std::stoi(value.value);
            }
            break; case ValueType::Member:
            {
                // Make sure the variable exists.
                if (variables.count(value.value) == 0)
                {
                    report_error("Variable '" + value.value + "' not found.");
                    return 0;
                }

                auto& variable = variables.at(value.value);

                // Check if the value exists as a member.
                if (!variable.values.contains(value.member))
                {
                    report_error("Member '" + value.member + "' not found in '" + value.value + "'.");
                    return 0;
                }

                // Now we get the pin number and set it. 
                const auto& pin = variable.chip_info->meta->get_pin(value.member);
                const auto& bus = variable.chip_info->meta->get_bus(value.member);

                if (bus.has_value())
                {
                    const auto& [name, start, size] = bus.value();
                    const auto& pins = variable.chip->input_pins;

                    std::size_t so = 0; 
                    if (start < MAX_INPUT_PINS) 
                    {
                        for (std::size_t offset = 0; offset < size; offset++)
                        {
                          so <<= 1;
                          so |= (variable.chip->input_pins.at(start + (size-1) - offset).is_active() ? 1 : 0);
                        }
                                
                    }
                    else
                    {
                        for (std::size_t offset = 0; offset < size; offset++)
                        {
                          so <<= 1;
                          so |= (variable.chip->output_pins.at(start + (size-1) - offset - MAX_INPUT_PINS).is_active() ? 1 : 0);
                        }
                    }


                    return so;
                }
                else if (pin.has_value())
                {
                    const auto pin_number = pin.value().pin_number;
                    return (variable.chip->get_pin(pin_number)->is_active()) ? 1 : 0;
                }

                // Should be unreachable.
                report_error(value.value + value.member + " is not of type bus or pin.");
                return variable.values[value.member];
            }
        }
        // Should be unreachable.
        report_error("Something went VERY wrong.");
        return 0;
    }
    
    auto get_representation(const Value& value) noexcept -> std::string 
    {
        switch (value.type)   
        {
            break; case ValueType::Number:
            {
                return value.value;
            }
            break; case ValueType::Member:
            {
                return value.value + "." + value.member;
            }
        }
        // Should be unreachable.
        report_error("Something went VERY wrong.");
        return "";
    }

    auto SET_impl(const Value& var, const Value& value) noexcept -> void
    {
        if (var.type == ValueType::Number)
        {
            report_error("Invalid SET statement, cannot set a constant value.");
            return;
        }

        // Make sure the variable exists.
        if (variables.count(var.value) == 0)
        {
            report_error("Variable '" + var.value + "' not found.");
            return;
        }

        auto& variable = variables.at(var.value);

        // Check if the value exists as a member.
        if (!variable.values.contains(var.member))
        {
            report_error("Member '" + var.member + "' not found in '" + var.value + "'.");
            return;
        }

        auto int_val = get_value(value);
        variable.values[var.member] = int_val;

        // Now we get the pin number and set it. 
        const auto& pin = variable.chip_info->meta->get_pin(var.member);
        const auto& bus = variable.chip_info->meta->get_bus(var.member);

        if (bus.has_value())
        {
            const auto& [name, start, size] = bus.value();
            auto& pins = variable.chip->input_pins;

            if ((int_val >> size) > 0)
            {
                report_error("Bus overflow. Max: " + std::to_string(_priv::pow2(size)-1) + ", got: " + std::to_string(int_val));
                return;
            }

            for (std::size_t offset = 0; offset < size; offset++)
            {
                pins[start + offset].state = (((int_val >> offset ) & 1) == 1) ? PinState::ACTIVE : PinState::INACTIVE;
                log("Setting " + name + "[" + std::to_string(start+offset) + "] = " + std::to_string(((int_val >> offset ) & 1) == 1 ? 1 : 0));
            }
        }
        else if (pin.has_value())
        {
            const auto pin_number = pin.value().pin_number;

            variable.chip->input_pins[pin_number].state = (int_val == 1) ? PinState::ACTIVE : PinState::INACTIVE;
        }
        else
        {
             report_error("Pin '" + var.member + "' not found in meta data.");
        }

    }

    auto SET_statement() noexcept -> void
    {
        log("Parsing SET statement.");

        // Parse variable.
        const auto var = parse_variable();

        consume(TestTokenType::Equal, "Expected assignment in SET statement.");

        const auto val = parse_variable();

        expect_semicolon("Expected ';' at the end of SET statement.");

        SET_impl(var, val);

        log("Finished parsing SET statement.");
    }

    auto parse_condition() noexcept -> Condition
    {
        auto grouped = (match(TestTokenType::LParen));

        const auto var = parse_variable();

        ConditionType type{};

        if (match(TestTokenType::Is))
        {
            type = ConditionType::IS;
        }
        else if (match(TestTokenType::Not))
        {
            type = ConditionType::NOT;
        }
        else
        {
            report_error("Expected comparison operator in condition.");
        }

        const auto val = parse_variable();

        if (grouped)
        {
            consume(TestTokenType::RParen, "Expected ')' at the end of grouped condition.");
        }

        return { .a=var, .b=val, .type=type };
    }

    auto REQUIRE_impl(std::vector<Condition>& conditions) noexcept -> void
    {
        const auto& test_name = board.context().first;
        bool expected = true;
        for (const auto& cond : conditions)
        {
            switch (cond.type)
            {
                break; case ConditionType::IS:
                {
                    expected = (get_value(cond.a)==get_value(cond.b));
                }
                break; case ConditionType::NOT:
                {
                    expected = (get_value(cond.a)!=get_value(cond.b));
                }
            }

            if (!expected)
            {
              test_failed = true;
              auto cond_op = cond.type == ConditionType::IS ? "==" : "!=";
              std::stringstream ss;

              ss << "[ Line " << std::to_string(current.line) << " ] REQUIRE " 
                 << get_representation(cond.a) 
                 << " " 
                 << cond_op 
                 << " "
                 << get_representation(cond.b)
                 << " -> "
                 << "\033[1;31m"
                 << "REQUIRE "
                 << get_value(cond.a) 
                 << " " 
                 << cond_op 
                 << " "
                 << get_value(cond.b)
                 << "\033[0m";

              failed_messages.emplace_back(ss.str());
              return;
            }
        }
    }

    auto REQUIRE_statement() noexcept -> void
    {
        log("Parsing REQUIRE statement.");

        std::vector<Condition> conditions;

        if (auto cond = parse_condition(); !has_error)
        {
            conditions.push_back(std::move(cond));
        }

        while (match(TestTokenType::And))
        {
            if (auto cond = parse_condition(); !has_error)
            {
                conditions.push_back(std::move(cond));
            }
        }

        expect_semicolon("Expected ';' at the end of REQUIRE statement.");

        if (!has_error)
            REQUIRE_impl(conditions);

        log("Finished parsing REQUIRE statement.");
    }

    auto parse_TEST_body() noexcept -> void
    {
        log("Parsing TEST body");

        consume(TestTokenType::LBrace, "Expected '{' at the beginning of TEST body.");

        while (!match(TestTokenType::RBrace))
        {
            if (match(TestTokenType::Var))
            {
                VAR_statement();
            }
            else if (match(TestTokenType::Set))
            {
                SET_statement();
            }
            else if (match(TestTokenType::Eval))
            {
                EVAL_statement();
            }
            else if (match(TestTokenType::Require))
            {
                REQUIRE_statement();
            }
            else if (match(TestTokenType::EndOfFile))
            {
                report_error("CHIP definition not terminated, expected '}', found '" +
                             current.lexeme + "'.");
                return;
            }
            else
            {
                // This will skip the rest of the body.
                restabilize();   
            }
        }
        log("Finished parsing TEST body");
    }

    auto TEST_impl(const std::string& test) noexcept -> void
    {
        test_failed = false;
        purge_variables();
        board.reset_context();
        board.create_new(test);
        board.set_context(test);
    }

    auto TEST_statement() noexcept -> void
    {
        log("Parsing TEST statement.");

        // Retrieve the test name.
        consume(TestTokenType::Quote, "Expected test name");
        const auto test_name = parse_quote();
        log("Test name: " + test_name);

        TEST_impl(test_name);

        // Parsing body.
        parse_TEST_body();

        const auto passed = !(test_failed || has_error);
        const auto status = passed ? "\033[1;32mPASSED \033[0m" : "\033[1;31mFAILED \033[0m";
        std::cout << "[ Test: " << test_name << "] " << status << '\n';

        if (!failed_messages.empty())
        {
            std::cout << "    [ Required Failed ]\n";
            for (const auto& message : failed_messages)
            {
                std::cout << "    " << message << '\n';
            }
            failed_messages.clear();
        }

        log("Finished parsing TEST statement.");
    }

    auto restabilize() noexcept -> void
    {
        // Reset panic flag, since now we can report a new error for a different part.
        this->panic = false;

        log("Restabilizing...");

        while (this->current.type != TestTokenType::EndOfFile)
        {
            switch (this->current.type)
            {
                case TestTokenType::Test:
                    return;
                default:; // Do nothing...
            }
            this->advance();
        }
    }

    auto purge_variables() noexcept -> void
    {
        variables.clear();   
    }

private:
    Board                           board;
    bool                            test_failed;
    std::map<std::string, ChipInfo> chip_images;
    std::map<std::string, Variable> variables; 
    std::vector<std::string>        failed_messages;
};

} /* namespace test */

#endif /* TESTER_H */
