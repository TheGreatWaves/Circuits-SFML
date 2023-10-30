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

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "parser_base.hpp"
#include "recipe_builder.hpp"
#include "scanner.hpp"

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
            log("Stabilizing...");
            restabilize();
            log("Restabilized...");
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

        log("Parsing identifier: " + previous.lexeme + ".");

        while (match(TokenType::COMMA))
        {
            consume(TokenType::IDENT, "Expected identifier, found '" + current.lexeme + "'.");
            builder.add_input_pin(previous.lexeme);
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
        log("Parsing identifier: " + previous.lexeme + ".");

        while (match(TokenType::COMMA))
        {
            consume(TokenType::IDENT, "Expected identifier, found '" + current.lexeme + "'.");
            builder.add_output_pin(previous.lexeme);
            log("Parsing identifier: " + previous.lexeme + ".");
        }
        consume(TokenType::SEMICOLON,
                "Expected ';' at the end of an OUT statement, found '" + current.lexeme + "'.");

        log("Finished parsing OUT statement.");
    }

    auto SUBGATE_statement() noexcept -> void
    {
        log("Parsing identifier: " + previous.lexeme + ".");
        builder.add_dependency(previous.lexeme);

        // Parsing the parameters of the subgate.
        consume(TokenType::LPAREN,
                "Expected left parenthesis, found '" + current.lexeme + "'.");

        // We expect ATLEAST one linkage.
        consume(TokenType::IDENT, "Linkage statement expects atleast one identifier.");
    log("Link in " + previous.lexeme + ".");
        consume(TokenType::ASSIGNMENT,
                "Expected assignment operator, found '" + current.lexeme + "'.");
        consume(TokenType::IDENT,
                "Linkage statement expected output, found '" + current.lexeme + "'.");
    log("Link out " + previous.lexeme + ".");

        while (match(TokenType::COMMA))
        {
            consume(TokenType::IDENT, "Linkage statement expects atleast one identifier.");
            log("Link in " + previous.lexeme + ".");
            consume(TokenType::ASSIGNMENT,
                    "Expected assignment operator, found '" + current.lexeme + "'.");
            consume(TokenType::IDENT,
                    "Linkage statement expected output, found '" + current.lexeme + "'.");
            log("Link out " + previous.lexeme + ".");
        }
        consume(TokenType::RPAREN,
                "Expected closing parenthesis, found '" + current.lexeme + "'.");
        consume(TokenType::SEMICOLON, "Expected ';' at the end of an PARTS statement, found '" +
                                          current.lexeme + "'.");
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
        builder.add_dependency(previous.lexeme);

        log("Parsing identifier: " + previous.lexeme + ".");

        // Parsing the parameters of the subgate.
        consume(TokenType::LPAREN, "Expected left parenthesis, found '" + current.lexeme + "'.");

        // We expect ATLEAST one linkage.
        consume(TokenType::IDENT, "Linkage statement expects atleast one identifier.");
        log("Link in " + previous.lexeme + ".");
        consume(TokenType::ASSIGNMENT,
                "Expected assignment operator, found '" + current.lexeme + "'.");
        consume(TokenType::IDENT,
                "Linkage statement expected output, found '" + current.lexeme + "'.");
        log("Link out " + previous.lexeme + ".");

        while (match(TokenType::COMMA))
        {
            consume(TokenType::IDENT, "Linkage statement expects atleast one identifier.");
            log("Link in " + previous.lexeme + ".");
            consume(TokenType::ASSIGNMENT,
                    "Expected assignment operator, found '" + current.lexeme + "'.");
            consume(TokenType::IDENT,
                    "Linkage statement expected output, found '" + current.lexeme + "'.");
            log("Link out " + previous.lexeme + ".");
        }
        consume(TokenType::RPAREN, "Expected closing parenthesis, found '" + current.lexeme + "'.");
        consume(TokenType::SEMICOLON,
                "Expected ';' at the end of an PARTS statement, found '" + current.lexeme + "'.");

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

  private:
    /**
     * Members.
     */
    RecipeBuilder builder;
};

} /* namespace hdl */

#endif /* HDL_PARSER */
