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
#ifndef HDL_PARSER_BASE
#define HDL_PARSER_BASE

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
class BaseParser
{
  public:
    /**
     * Constructor with file path of HDL source code.
     */
    [[nodiscard]] explicit BaseParser(const std::string& file_path)
    {
        if (!scanner.read_source(file_path))
            throw std::invalid_argument("File: " + file_path + " not found!");
    }

    /**
     * Default Ctor prohibted.
     */
    constexpr BaseParser() = delete;

    /**
     * Protected methods.
     */
  protected:
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
     * Members.
     */
  protected:
    Token         current;
    Token         previous;
    Scanner       scanner;

    bool panic{false};
    bool has_error{false};
};

} /* namespace hdl */

#endif /* HDL_PARSER_BASE */
