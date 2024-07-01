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

#ifndef TOKEN_DESCRIPTOR_FILE
#error "TOKEN_DESCRIPTOR_FILE not specified."
#else

#ifndef TOKEN_CLASS_NAME
#error "TOKEN_CLASS_NAME not specified."
#endif

#include <cctype>
#include <fstream>
#include <sstream>
#include <string>

#include "comptrie.hpp"
#include "token.hpp"

#ifndef JOIN
#define JOIN(a, b) a##b
#endif

#ifndef SCANNER
#define SCANNER(name) JOIN(name, Scanner)
#endif

/**
 * The Scanner class is responsible for
 * processing the input source code into
 * tokens.
 */
struct SCANNER(TOKEN_CLASS_NAME)
{
  public:

    // JOIN(TOKEN_CLASS_NAME, Scanner)() = default;
    SCANNER(TOKEN_CLASS_NAME)() = default;

    

    void set_source(const std::string& source)
    {
        reset();
        source_code = source;
    }

    /**
     * Read all the source code from the file.
     */
    bool read_source(const std::string& path)
    {
        reset();

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
    [[nodiscard]] Token<TOKEN_CLASS_NAME> scan_number() noexcept
    {
        while (std::isdigit(peek())) advance_position();
        return make_token(TOKEN_CLASS_NAME::Number);
    }

    /**
     * Keep consuming while the character is alphanumeric.
     */
    [[nodiscard]] Token<TOKEN_CLASS_NAME> scan_identifier() noexcept
    {
        while (std::isalnum(peek()) || peek()=='_') advance_position();
        return make_token(identifier_type());
    }

    /**
     * Return the identifer type. This could return as
     * one of the keyword types.
     */
    [[nodiscard]] auto identifier_type() -> TOKEN_CLASS_NAME 
    {
        const auto word = source_code.substr(start, current-start);
        
        #define KEYWORD_TOKEN(name, symbol) if(symbol==word) return TOKEN_CLASS_NAME::name;
        #include TOKEN_DESCRIPTOR_FILE

        return TOKEN_CLASS_NAME::Identifier;
    }

    /**
     * Return the next token.
     */
    [[nodiscard]] Token<TOKEN_CLASS_NAME> scan_token() noexcept
    {
        skip_whitespace();
        start = current;

        if (is_at_end()) 
        {
            return make_token(TOKEN_CLASS_NAME::EndOfFile);
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
            #define SYMBOL_TOKEN(name, symbol) break; case symbol[0]: return make_token(TOKEN_CLASS_NAME::name);
            #include TOKEN_DESCRIPTOR_FILE
        }
        #undef CCASE

        return error_token("Unexpected character.");
    }

    /**
     * Returns true when the current position is
     * the same as the source code's length.
     */
    [[nodiscard]] bool is_at_end() noexcept
    {
        return current >= this->source_code.length();
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

    void reset() noexcept
    {
        start = 0;
        current = start;
        line = 1;
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
    [[nodiscard]] Token<TOKEN_CLASS_NAME> make_token(const TOKEN_CLASS_NAME type) noexcept 
    {
        const std::size_t length = current - start;
        return Token(type, this->source_code.substr(start, length), this->line);
    }

    /**
     * Create an error token.
     */
    [[nodiscard]] Token<TOKEN_CLASS_NAME> error_token(const std::string& message) noexcept 
    {
        return Token(TOKEN_CLASS_NAME::Error, message, this->line);
    }

  private:
    std::size_t start{0};
    std::size_t current{start};
    std::string source_code{};
    std::size_t line{1};
};

#endif

#undef TOKEN_DESCRIPTOR_FILE
#undef TOKEN_CLASS_NAME

#undef SCANNER
