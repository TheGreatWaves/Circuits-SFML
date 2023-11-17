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
#ifndef HDL_PARSER
#define HDL_PARSER

// #define DEBUG

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <map>
#include <memory>

#include "parser_base.hpp"
#include "recipe_builder.hpp"
#include "scanner.hpp"
#include "meta.hpp"

namespace hdl
{

/**
 * The parser for the HDL. This parser does not form an AST, it will
 * directly form a RecipeBuilder object, which can then be executed.
 * With the current design, a single parser is only intended to parse
 * a single file with a single CHIP declaration.
 */
class HDLParser : public BaseParser
{
  public:
    /**
     * Constructor with file path of HDL source code.
     */
    [[nodiscard]] explicit HDLParser(const std::string& file_path)
        : BaseParser(file_path)
    {
    }

    /**
     * Default Ctor prohibted.
     */
    constexpr HDLParser() = delete;

    /**
     * Parse the source code and build the RecipeBuilder.
     */
    [[nodiscard]] auto parse() noexcept -> bool
    {
        advance();

        while (!match(TokenType::END))
        {
            declaration();
        }

        return !this->has_error;
    }

    [[nodiscard]] auto result() const noexcept -> const RecipeBuilder&
    {
        return builder;
    }

    /**
     * Parsing functions.
     */
  private:
    auto declaration() noexcept -> void
    {
        if (match(TokenType::IN))
        {
            IN_statement();
        }
        else if (match(TokenType::OUT))
        {
            OUT_statement();
        }
        else if (match(TokenType::PARTS))
        {
            PARTS_statement();
        }
        else if (match(TokenType::IDENT))
        {
            SUBGATE_statement();
        }
        else if (match(TokenType::CHIP))
        {
            CHIP();
        }

        if (this->panic)
        {
            restabilize();
        }
    }

    /**
     * Parsing an IN (input) statement.
     */
    auto IN_statement() noexcept -> void
    {
        log("Parsing IN statement.");
        // We expect at least one identifier.
        consume(TokenType::IDENT, "IN statement expects atleast one identifier.");
        builder.add_input_pin(previous.lexeme);
        pin_numbers[previous.lexeme] = input_pin_offset++;

        log("Parsing identifier: " + previous.lexeme + ".");

        while (match(TokenType::COMMA))
        {
            consume(TokenType::IDENT, "Expected identifier, found '" + current.lexeme + "'.");
            builder.add_input_pin(previous.lexeme);
            pin_numbers[previous.lexeme] = input_pin_offset++;
            log("Parsing identifier: " + previous.lexeme + ".");
        }
        consume(TokenType::SEMICOLON,
                "Expected ';' at the end of an IN statement, found '" + current.lexeme + "'.");

        log("Finished parsing IN statement.");
    }

    /**
     * Parsing an OUT (output) statement.
     */
    auto OUT_statement() noexcept -> void
    {
        log("Parsing OUT statement.");
        // We expect at least one identifier.
        consume(TokenType::IDENT, "OUT statement expects atleast one identifier.");
        builder.add_output_pin(previous.lexeme);
        pin_numbers[previous.lexeme] = MAX_INPUT_PINS + output_pin_offset++;
        log("Parsing identifier: " + previous.lexeme + ".");

        while (match(TokenType::COMMA))
        {
            consume(TokenType::IDENT, "Expected identifier, found '" + current.lexeme + "'.");
            builder.add_output_pin(previous.lexeme);
            pin_numbers[previous.lexeme] = MAX_INPUT_PINS + output_pin_offset++;
            log("Parsing identifier: " + previous.lexeme + ".");
        }
        consume(TokenType::SEMICOLON,
                "Expected ';' at the end of an OUT statement, found '" + current.lexeme + "'.");

        log("Finished parsing OUT statement.");

    }

    auto SUBGATE_statement() noexcept -> void
    {
        log("Parsing identifier: " + previous.lexeme + ".");
        auto subgate_name = previous.lexeme;
        builder.add_dependency(subgate_name);
        auto subgate_added = add_subgate_metadata(subgate_name);
        if (!subgate_added)
        {
            report_error("Could not retrieve metadata for '" + previous.lexeme + "'.");
        }
        else
        {
            log("Getting context for " + previous.lexeme + " gate.");
            this->context_gate_metadata = subgate_metadata.at(previous.lexeme).get();
        }


        log("Parsing identifier: " + previous.lexeme + ".");

        // Parsing the parameters of the subgate.
        consume(TokenType::LPAREN, "Expected left parenthesis, found '" + current.lexeme + "'.");

        // We expect ATLEAST one linkage.
        consume(TokenType::IDENT, "Linkage statement expects atleast one identifier.");
        const auto subgate_input_pin = previous.lexeme;
        consume(TokenType::ASSIGNMENT, "Expected assignment operator, found '" + current.lexeme + "'.");
        consume(TokenType::IDENT, "Linkage statement expected output, found '" + current.lexeme + "'.");
        const auto subgate_output_name = previous.lexeme;

        if (subgate_added)
        {
            if (auto pin = context_gate_metadata->get_pin(subgate_input_pin); pin.has_value())
            {
                log("PIN NUMBER: " + std::to_string((*pin).pin_number));
                const auto is_output = (*pin).pin_number >= MAX_INPUT_PINS;
                const auto offset = is_output ? output_pin_offset : input_pin_offset;
                const auto pin_number = (*pin).pin_number + offset;
                const auto linkage_entry = Meta::PinEntry{subgate_output_name, pin_number};
                linkages.emplace_back(linkage_entry, is_output);

                // Declare a new output pin entry if not found.

                auto output_pin_not_found = (pin_numbers.count(subgate_output_name) == 0);
                if (output_pin_not_found)
                {
                    pin_numbers[subgate_output_name] = pin_number;
                }
            }
            else
            {
                report_error("Pin '" + subgate_input_pin + "' not found in '" + subgate_name + "'.");
            }
        }

        while (match(TokenType::COMMA))
        {
            consume(TokenType::IDENT, "Linkage statement expects atleast one identifier.");
            const auto subgate_input_pin = previous.lexeme;
            consume(TokenType::ASSIGNMENT, "Expected assignment operator, found '" + current.lexeme + "'.");
            consume(TokenType::IDENT, "Linkage statement expected output, found '" + current.lexeme + "'.");
            const auto subgate_output_name = previous.lexeme;

            if (subgate_added)
            {
                if (auto pin = context_gate_metadata->get_pin(subgate_input_pin); pin.has_value())
                {
                    const auto is_output = (*pin).pin_number >= MAX_INPUT_PINS;
                    const auto offset = is_output ? output_pin_offset : input_pin_offset;
                    const auto pin_number = (*pin).pin_number + offset;
                    const auto linkage_entry = Meta::PinEntry{subgate_output_name, pin_number};

                    // Declare a new output pin entry if not found.

                    const auto output_pin_not_found = (pin_numbers.count(subgate_output_name) == 0);
                    if (output_pin_not_found)
                    {
                        pin_numbers[subgate_output_name] = pin_number;
                    }
                    else
                    {
                        linkages.emplace_back(linkage_entry, is_output);
                    }
                }
                else
                {
                    report_error("Pin '" + subgate_input_pin + "' not found in '" + subgate_name + "'.");
                }
            }
        }

        consume(TokenType::RPAREN, "Expected closing parenthesis, found '" + current.lexeme + "'.");
        consume(TokenType::SEMICOLON,
                "Expected ';' at the end of an PARTS statement, found '" + current.lexeme + "'.");

        // Increment the offset accordingly.
        input_pin_offset += context_gate_metadata->input_count;
        output_pin_offset += context_gate_metadata->output_count;
    }

    /**
     * Parsing parts.
     */
    auto PARTS_statement() noexcept -> void
    {
        log("Parsing PARTS statement.");

        consume(TokenType::COLON, "Expected ':', found '" + current.lexeme + "'.");

        // Subcomponents are NOT optional. A gate without subgates would be useless.
        // We expect atleast one identifier.
        consume(TokenType::IDENT, "PARTS statement expects atleast one identifier.");
        auto subgate_name = previous.lexeme;
        builder.add_dependency(subgate_name);
        auto subgate_added = add_subgate_metadata(previous.lexeme);
        if (!subgate_added)
        {
            report_error("Could not retrieve metadata for '" + previous.lexeme + "'.");
        }
        else
        {
            this->context_gate_metadata = subgate_metadata.at(previous.lexeme).get();
        }


        log("Parsing identifier: " + previous.lexeme + ".");

        // Parsing the parameters of the subgate.
        consume(TokenType::LPAREN, "Expected left parenthesis, found '" + current.lexeme + "'.");

        // We expect ATLEAST one linkage.
        consume(TokenType::IDENT, "Linkage statement expects atleast one identifier.");
        const auto subgate_input_pin = previous.lexeme;
        consume(TokenType::ASSIGNMENT, "Expected assignment operator, found '" + current.lexeme + "'.");
        consume(TokenType::IDENT, "Linkage statement expected output, found '" + current.lexeme + "'.");
        const auto subgate_output_name = previous.lexeme;

        if (subgate_added)
        {
            if (auto pin = context_gate_metadata->get_pin(subgate_input_pin); pin.has_value())
            {
                log("PIN NUMBER: " + std::to_string((*pin).pin_number));
                const auto is_output = (*pin).pin_number >= MAX_INPUT_PINS;
                const auto offset = is_output ? output_pin_offset : input_pin_offset;
                const auto pin_number = (*pin).pin_number + offset;
                const auto linkage_entry = Meta::PinEntry{subgate_output_name, pin_number};

                // Declare a new output pin entry if not found.

                const auto output_pin_not_found = (pin_numbers.count(subgate_output_name) == 0);
                if (output_pin_not_found)
                {
                    log("Declaring new output gate: " + subgate_output_name + " at pin: " + std::to_string(MAX_INPUT_PINS + output_pin_offset + 1)); 
                    pin_numbers[subgate_output_name] = pin_number;
                }
                else
                {
                    linkages.emplace_back(linkage_entry, is_output);
                }
            }
            else
            {
                report_error("Pin '" + subgate_input_pin + "' not found in '" + subgate_name + "'.");
            }
        }

        while (match(TokenType::COMMA))
        {
            consume(TokenType::IDENT, "Linkage statement expects atleast one identifier.");
            const auto subgate_input_pin = previous.lexeme;
            consume(TokenType::ASSIGNMENT, "Expected assignment operator, found '" + current.lexeme + "'.");
            consume(TokenType::IDENT, "Linkage statement expected output, found '" + current.lexeme + "'.");
            const auto subgate_output_name = previous.lexeme;

            if (subgate_added)
            {
                if (auto pin = context_gate_metadata->get_pin(subgate_input_pin); pin.has_value())
                {
                    const auto is_output = (*pin).pin_number >= MAX_INPUT_PINS;
                    const auto offset = is_output ? output_pin_offset : input_pin_offset;
                    const auto pin_number = (*pin).pin_number + offset;
                    const auto linkage_entry = Meta::PinEntry{subgate_output_name, pin_number};

                    // Declare a new output pin entry if not found.

                    const auto output_pin_not_found = (pin_numbers.count(subgate_output_name) == 0);
                    if (output_pin_not_found)
                    {
                        log("Declaring new output gate: " + subgate_output_name + " at pin: " + std::to_string(MAX_INPUT_PINS + output_pin_offset + 1)); 
                        pin_numbers[subgate_output_name] = pin_number;
                    }
                    else
                    {
                        linkages.emplace_back(linkage_entry, is_output);
                    }
                }
            }
            else
            {
                report_error("Pin '" + subgate_input_pin + "' not found in '" + subgate_name + "'.");
            }
        }

        consume(TokenType::RPAREN, "Expected closing parenthesis, found '" + current.lexeme + "'.");
        consume(TokenType::SEMICOLON, "Expected ';' at the end of an PARTS statement, found '" + current.lexeme + "'.");

        // Increment the offset accordingly.
        input_pin_offset += context_gate_metadata->input_count;
        output_pin_offset += context_gate_metadata->output_count;

        while (match(TokenType::IDENT))
        {
            SUBGATE_statement();
        }

        log("Finished parsing PARTS statement.");
    }

    /**
     * Parse CHIP declaration.
     */
    auto CHIP() noexcept -> void
    {
        log("Parsing CHIP declaration.");

        // Parse the chip name.
        consume(TokenType::IDENT, "Expected CHIP name, found '" + current.lexeme + "'.");
        builder.set_gate_name(previous.lexeme);

        // Beginning the definition.
        consume(TokenType::LBRACE, "CHIP declaration expected definition block, missing '{', found '" + current.lexeme + "'.");

        while (!match(TokenType::RBRACE))
        {
            if (match(TokenType::IN))
            {
                IN_statement();
            }
            else if (match(TokenType::OUT))
            {
                OUT_statement();
            }
            else if (match(TokenType::PARTS))
            {
                PARTS_statement();
            }
            else if (match(TokenType::END))
            {
                report_error("CHIP definition not terminated, expected '}', found '" + current.lexeme + "'.");
                return;
            }
        }

        // Once we finish parsing the parts, we can now apply the linkages.
        for (const auto& [linkage, is_output] : linkages)
        {
            if (pin_numbers.count(linkage.pin_name) != 0)
            {
                const auto pin_number = pin_numbers.at(linkage.pin_name);

                if (is_output) // If output, we swap the ordering.
                {
                    log("Linkage: " + std::to_string(linkage.pin_number) + " <--> " + std::to_string(pin_number));
                    builder.add_wire_linkage(linkage.pin_number, pin_number);
                }
                else
                {
                    log("Linkage: " + std::to_string(pin_number) + " <--> " + std::to_string(linkage.pin_number));
                    builder.add_wire_linkage(pin_number, linkage.pin_number);
                }
            }
            else
            {
                report_error("Linkage: " + std::to_string(linkage.pin_number) + " <--> UNKNOWN");
            }
        }

        log("Finished parsing CHIP declaration.");
    }

    /**
     * We restabilize at the next declaration, by doing this, we can report more than
     * a single in one parse, instead of having to parse over and over and returning
     * only a single error back at a time.
     */
    auto restabilize() noexcept -> void
    {
        // Reset panic flag, since now we can report a new error for a different part.
        this->panic = false;

        log("Restabilizing...");

        while (this->current.type != TokenType::END)
        {
            auto token = "Token: " + current.lexeme + ", " + std::to_string(static_cast<int>(current.type));
            log(token);
            if (this->previous.type == TokenType::SEMICOLON)
                return;

            switch (this->current.type)
            {
                case TokenType::CHIP:
                case TokenType::IN:
                case TokenType::OUT:
                case TokenType::PARTS:
                    return;
                default:; // Do nothing...
            }
            this->advance();
        }
    }

    [[nodiscard]] auto add_subgate_metadata(const std::string& subgate) noexcept -> bool 
    {
        if (subgate_metadata.count(subgate) == 0) 
        {
            auto gate = Meta::get_meta(subgate);
            if (gate == nullptr)
            {
                return false;
            }
            subgate_metadata[subgate] = std::move(gate);
        }
        return true;
    }

  private:
    /**
     * Members.
     */
    RecipeBuilder                                      builder{};
    std::size_t                                        input_pin_offset{};
    std::size_t                                        output_pin_offset{};
    const Meta*                                        context_gate_metadata{};
    std::vector<std::pair<Meta::PinEntry, bool>>       linkages{};
    std::map<std::string, std::size_t>                 pin_numbers{};
    std::map<std::string, std::unique_ptr<const Meta>> subgate_metadata{};
};

} /* namespace hdl */

#endif /* HDL_PARSER */
