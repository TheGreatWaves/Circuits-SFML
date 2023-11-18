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
#ifndef RAW_PARSER
#define RAW_PARSER


#include "token_raw.hpp"
#include "parser_base.hpp"

/**
 * The RawParser class simply exposes the protected methods of the base parser class.
 * This allows it to be used in any context easily. It is a struct because everything
 * is expected to be public.
 */
struct RawParser : public BaseParser<RawTokenTypeScanner, RawTokenType>
{
    /**
     * Constructor with file path of HDL source code.
     */
    [[nodiscard]] explicit RawParser(const std::string& file_path)
        : BaseParser(file_path)
    {
    }

    /**
     * Default Ctor prohibted.
     */
    constexpr RawParser() 
     : BaseParser()
    {
    }

    auto set_source (const std::string& source) -> void
    {
     scanner.set_source(source);
    }

    /**
     * Retrieve the current token.
     */
    [[nodiscard]] auto get_current() const noexcept -> Token<RawTokenType>
    {
        return this->current;
    }

    /**
     * Return the error status of the parser.
     */
    [[nodiscard]] auto get_error() const noexcept -> bool
    {
      return this->has_error;
    }

    /**
     *  Retrieve the previous token.
     */
    [[nodiscard]] auto get_previous() const noexcept -> Token<RawTokenType>
    {
        return this->previous;
    }

    /**
     * Check the current token type, if it matches what expect, we consume.
     * This consumption is optional, an error will not be thrown. Returns
     * true if the token was consumed.
     */
    [[nodiscard]] auto match_token(RawTokenType type) noexcept -> bool
    {
        return match(type);
    }

    /**
     * Consume the current token if it matches what we expect,
     * else we report error.
     */
    auto consume_token(RawTokenType type, std::string_view message) noexcept -> void
    {
        consume(type, message);
    }

    /**
     * Advance and return the current token.
     */
    [[nodiscard]] auto advance_token() noexcept -> Token<RawTokenType>
    {
        advance();
        return current;
    }

    auto report_custom_error(std::string_view message) noexcept -> void
    {
        report_error(message);
    }
};

#endif /* RAW_PARSER */
