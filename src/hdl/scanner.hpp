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
#ifndef HDL_SCANNER_H
#define HDL_SCANNER_H

#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>

#include "token.hpp"
#include "../lang/core/comptrie.hpp"

namespace hdl
{

/**
 * The Scanner class is responsible for
 * processing the input source code into
 * tokens.
 */
struct Scanner
{
  public:

    /**
     * Default constructor.
     * Hardcoded trie.
     */ 
    Scanner()
    {
    }

    /**
     * Read all the source code from the file.
     */
    bool read_source(const std::string& path)
    {
        this->source_code.clear();

        std::ifstream     ifs(path);
        if (ifs.fail()) return false;

        std::stringstream buffer;
        buffer << ifs.rdbuf();
        this->source_code = buffer.str();

        return true;
    }

    /**
     * Keep consuming while the character is a digit.
     */
    [[nodiscard]] Token scan_number() noexcept
    {
        while (std::isdigit(peek())) advance_position();
        return make_token(TokenType::NUMBER);
    }

    /**
     * Keep consuming while the character is alphanumeric.
     */
    [[nodiscard]] Token scan_identifier() noexcept
    {
        while (std::isalnum(peek()) || peek()=='_') advance_position();
        return make_token(identifier_type());
    }

    /**
     * Return the identifer type. This could return as
     * one of the keyword types.
     */
    [[nodiscard]] auto identifier_type() -> TokenType 
    {
        auto word = source_code.substr(start, current-start);
        
        return MATCH(word)
            return TokenType::IDENT;
        CASE("IN") return TokenType::IN;
        CASE("OUT") return TokenType::OUT;
        CASE("CHIP") return TokenType::CHIP;
        CASE("PARTS") return TokenType::PARTS;
        ENDMATCH;
    }

    /**
     * Return the next token.
     */
    [[nodiscard]] Token scan_token() noexcept
    {
        skip_whitespace();
        start = current;

        if (is_at_end()) 
        {
            return EOF_TOKEN;
        }

        const auto c = advance();

        if (std::isdigit(c))
        {
            return scan_number();
        }
        else if (std::isalpha(c)) 
        {
            return scan_identifier();
        }

        #define CCASE(character, token) break; case character: return make_token(token)
        switch (c)
        {
            CCASE('(', TokenType::LPAREN);
            CCASE(')', TokenType::RPAREN);
            CCASE('{', TokenType::LBRACE);
            CCASE('}', TokenType::RBRACE);
            CCASE(';', TokenType::SEMICOLON);
            CCASE(':', TokenType::COLON);
            CCASE(',', TokenType::COMMA);
            CCASE('=', TokenType::ASSIGNMENT);
        }
        #undef CASE

        return error_token("Unexpected character.");
    }

  private:
    /**
     * Return the current character and advance to the next.
     */
    const char advance() noexcept
    {
        return source_code.at(current++);
    }

    /**
     * Advance the position without returning anything.
     */
    void advance_position() noexcept
    {
        ++current;
    }

    /**
     * Take a peek at the character 'offset' amount away from current.
     * Offset is defaulted to 0, returning the current character without
     * changing the position of the current position.
     */
    const char peek(std::size_t offset = 0) noexcept
    {
        if (current + offset >= this->source_code.length())
            return '\0';
        return source_code.at(current + offset);
    }

    /**
     * Returns true when the current position is
     * the same as the source code's length.
     */
    [[nodiscard]] bool is_at_end() noexcept
    {
        return current >= this->source_code.length();
    }

    /**
     * Check if the current character matches the expected
     * character. If it does, we return true AND advance to
     * the next character.
     */
    [[nodiscard]] bool match(const char expected) noexcept
    {
        if (is_at_end() || peek() != expected)
            return false;
        ++current;
        return true;
    }

    /**
     * Consume all whitespace.
     */
    void skip_whitespace() noexcept
    {
        while (true)
        {
            switch (peek())
            {
                break; case '\n':
                {
                    ++line;
                }
                       case '\r':
                       case '\t':
                       case ' ':
                {
                    advance_position();
                }
                break; case '/':
                {
                    if (peek(1) == '/')
                    {
                        while(!is_at_end() && peek() != '\n')
                        {
                            advance_position();
                        }
                    }
                    else
                    {
                        return;
                    }
                }
                break; default: { return; }
            }
        }
    }

    /**
     * Create a token with the current string slice.
     */
    [[nodiscard]] Token make_token(const TokenType type) noexcept 
    {
        const std::size_t length = current - start;
        return Token(type, this->source_code.substr(start, length), this->line);
    }

    /**
     * Create an error token.
     */
    [[nodiscard]] Token error_token(const std::string& message) noexcept 
    {
        return Token(TokenType::ILLEGAL, message, this->line);
    }

  private:
    std::size_t start{0};
    std::size_t current{start};
    std::string source_code{};
    std::size_t line{1};
};

} /* namespace hdl */

#endif /* HDL_SCANNER_H */