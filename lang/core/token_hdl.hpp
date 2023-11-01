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
#ifndef TOKEN_HDL_H
#define TOKEN_HDL_H

#include <cstdint>

#include "enum_base.hpp"

/**
 * Declare new raw enum class and name values.
 */
DECLARE_RAW_ENUM_CLASS(HDLTokenKind, uint8_t) {
 #define TOKEN(name) RAW_ENUM_ENUMERATOR(name)
 #include "token_hdl.def"
};

/**
 * Subclass from EnumBase.
 */
class HDLTokenKind : public ENUM_BASE(HDLTokenKind)
{
public:
#define TOKEN(name) ENUM_CONSTANT_DECLARATION_ENUMERATOR(name)
#include "token_hdl.def"

 [[nodiscard]] constexpr auto is_symbol() const noexcept -> bool { return _is_symbol[as_int()]; }
 [[nodiscard]] constexpr auto is_keyword() const noexcept -> bool { return _is_keyword[as_int()]; }

 static const HDLTokenKind keyword_tokens[];
 static const HDLTokenKind all_tokens[];

private:
 // Keywords.
 static const bool _is_keyword[];

 // Symbols.
 static const bool _is_symbol[];
};

/**
 * Generate definitions.
 */
#define TOKEN(name) ENUM_CONSTANT_DEFINITION_ENUMERATOR(HDLTokenKind, name)
#include "token_hdl.def"

/**
 * Generate enum names.
 */
DEFINE_ENUM_CLASS_NAMES(HDLTokenKind) = {
#define TOKEN(name) ENUM_NAME_ENUMERATOR(name)
 #include "token_hdl.def"
};

/*================================*/
/*                                */
/*     Properties generation.     */
/*                                */
/*================================*/

// Marking keywords.
constexpr HDLTokenKind HDLTokenKind::keyword_tokens[] = {
 #define KEYWORD_TOKEN(name, symbol) HDLTokenKind::name,
 #include "token_hdl.def"
};

constexpr HDLTokenKind HDLTokenKind::all_tokens[] = {
 #define TOKEN(name) HDLTokenKind::name,
 #include "token_hdl.def"
};

// Defining keywords.
constexpr bool HDLTokenKind::_is_keyword[] = {
 #define TOKEN(name) false,
 #define KEYWORD_TOKEN(name, keyword) true,
 #include "token_hdl.def"
};

// Defining symbols.
constexpr bool HDLTokenKind::_is_symbol[] = {
 #define TOKEN(name) false,
 #define SYMBOL_TOKEN(name, symbol) true,
 #include "token_hdl.def"
};

#endif /* TOKEN_HDL_H */
