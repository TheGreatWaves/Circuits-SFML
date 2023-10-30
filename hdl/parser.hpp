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
class Parser
{
  public:
    /**
     * Constructor with file path of HDL source code.
     */
    [[nodiscard]] explicit Parser(const std::string& file_path)
    {
        if (!scanner.read_source(file_path))
            throw std::invalid_argument("File: " + file_path + " not found!");
    }

    /**
     * Default Ctor prohibted.
     */
    constexpr Parser() = delete;

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

        return this->has_error;
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
        log("Parsing identifier: " + previous.lexeme + ".");

        while (match(TokenType::COMMA))
        {
            consume(TokenType::IDENT, "Expected identifier, found '" + current.lexeme + "'.");
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
        log("Parsing identifier: " + previous.lexeme + ".");

        while (match(TokenType::COMMA))
        {
            consume(TokenType::IDENT, "Expected identifier, found '" + current.lexeme + "'.");
            log("Parsing identifier: " + previous.lexeme + ".");
        }
        consume(TokenType::SEMICOLON,
                "Expected ';' at the end of an OUT statement, found '" + current.lexeme + "'.");

        log("Finished parsing OUT statement.");
    }

    auto SUBGATE_statement() noexcept -> void
    {
        log("Parsing identifier: " + previous.lexeme + ".");

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
     * Private methods.
     */
  private:
    /**
     * Advance the current token.
     */
    auto advance() noexcept -> void
    {
        previous = current;

        while (true)
        {
            current = scanner.scan_token();
            if (current.type != TokenType::ILLEGAL)
            {
                break;
            }
            report_token_error(current);
        }
    }

    /**
     * Consume the current token if it matches what we expect,
     * else we report error.
     */
    auto consume(TokenType type, std::string_view message) noexcept -> void
    {
        if (current.type == type)
        {
            advance();
            return;
        }
        report_error(message);
    }

    /**
     * Check the current token type, if it matches what expect, we consume.
     * This consumption is optional, an error will not be thrown. Returns
     * true if the token was consumed.
     */
    [[nodiscard]] auto match(TokenType type) noexcept -> bool
    {
        if (current.type == type)
        {
            advance();
            return true;
        }
        return false;
    }

    /**
     * Returns true when the current type is equivalent to the expected type.
     */
    [[nodiscard]] bool check(TokenType type) const noexcept
    {
        return current.type == type;
    }

    /**
     * Report the token which caused an error.
     */
    auto report_token_error(Token token) noexcept -> void
    {
        auto message = "Unexpected token " + token.lexeme + '.';
        report_error(message);
    }

    /**
     * Report a generic error. (Custom error messasge)
     */
    auto report_error(std::string_view message) noexcept -> void
    {
        // Prevevent error message overload for one declaration.
        if (this->panic)
            return;

        this->panic = true;

        std::cout << "ERROR [ (line:" << previous.line << ") " << message << " ]\n";

        this->has_error = true;
    }

    /**
     * Log function, for debugging purposes.
     */
    auto log(std::string_view message) const noexcept -> void
    {
#ifdef DEBUG
        std::cout << "LOG [ " << message << " ]\n";
#else
#endif
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

    /**
     * Members.
     */
  private:
    Token         current;
    Token         previous;
    Scanner       scanner;
    RecipeBuilder builder;

    bool panic{false};
    bool has_error{false};
};

} /* namespace hdl */

#endif /* HDL_PARSER */
