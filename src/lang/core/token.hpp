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
#ifndef TOKEN_H
#define TOKEN_H

#include <string>

/**
 * A basic token. Used to classify text.
 */
template<typename TokenType>
struct Token
{
    /**
     * METHODS.
     */

    /**
     * Constructor.
     */
    [[nodiscard]] Token(TokenType t, std::string_view l, std::size_t line_num) noexcept
        : type{t}
        , lexeme{l}
        , line{line_num}
    {
    }

    /**
     * Default constructor, return EOF.
     */
    [[nodiscard]] Token() noexcept
        : type{TokenType::EndOfFile}
        , lexeme{"\0"}
    {
    }

    /**
     * MEMBERS.
     */
    TokenType   type{};
    std::string lexeme{};
    std::size_t line{};
};


#endif /* TOKEN_H */
