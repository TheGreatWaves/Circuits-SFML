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
#ifndef TOKEN_BASE
#define TOKEN_BASE

#include <string_view>
#include <type_traits>

/**
 * Inspired by the Carbon programming language enums.
 */

#define TOKEN_DECLARE_RAW_ENUM_CLASS(enum_class_name, underlying_type) \
    namespace _private                                                 \
    {                                                                  \
    enum class enum_class_name##Raw : underlying_type;                 \
    extern const std::string_view enum_class_name##Names[];            \
    }                                                                  \
    enum class _private::enum_class_name##Raw : underlying_type

// Represents an enum entry.
#define TOKEN_RAW_ENUM_ENUMERATOR(name) name,

#define TOKEN_DEFINE_ENUM_CLASS_NAMES(enum_class_name) \
inline constexpr std::string_view _private::enum_class_name##Names[]

// This can be used inside arrays to generate a list of enum names.
#define TOKEN_ENUM_NAME(name) #name,

// This is used to generate a named constant for each enum values.
#define TOKEN_ENUM_CONSTANT_DECLARATION(name) static const EnumType name;

#define TOKEN_ENUM_CONSTANT_DEFINITION(enum_class_name, name) \
 constexpr enum_class_name enum_class_name::name = enum_class_name::create(RawEnumType::name);

#define ENUM_BASE(enum_class_name) \
 EnumBase<enum_class_name, _private::enum_class_name##Raw, _private::enum_class_name##Names>

// The TokenBase class is simply a wrapper class for a enum type.
// It utilizes X-macros for ease of extension.
template <typename DerivedType, typename RawT, const std::string_view Names[]>
class EnumBase
{
  public:
    using RawEnumType    = RawT;
    using EnumType       = DerivedType;
    using UnderlyingType = std::underlying_type<RawT>;

    // Allow conversion to raw type.
    constexpr operator RawEnumType() const
    {
        return value;
    }

    [[nodiscard]] auto name() const -> const std::string_view
    {
        return Names[as_int()];
    }

  protected:
    constexpr EnumBase() = default;

    static constexpr auto create(RawEnumType v) -> EnumType
    {
        EnumType r;
        r.value = v;
        return r;
    }

    [[nodiscard]] constexpr auto as_int() const noexcept -> int
    {
        return static_cast<int>(value);
    }

  private:
    RawEnumType value;
};

#endif
