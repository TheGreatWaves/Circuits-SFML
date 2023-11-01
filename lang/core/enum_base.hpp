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
#ifndef ENUM_BASE_CLASS
#define ENUM_BASE_CLASS

#include <string_view>
#include <type_traits>

/**
 * This enum implementation technique is heaviliy inspired by the Carbon programming language enums.
 */

// Fixing macro pasting issue.
#define JOIN(x, y) x ## y

/**
 * Use to declare a new raw enum class. Note that this also define 
 * a list holding the name of each enum.
 */
#define DECLARE_RAW_ENUM_CLASS(enum_class_name, underlying_type)  \
    namespace _private                                            \
    {                                                             \
    enum class JOIN(enum_class_name, Raw) : underlying_type;      \
    extern const std::string_view JOIN(enum_class_name, Names)[]; \
    }                                                             \
    enum class _private::JOIN(enum_class_name, Raw) : underlying_type

/**
 * Defines a list of strings holding the name for
 * each entry of the enum. Note that this has to 
 * be called after declaring a new enum class.
 */
#define DEFINE_ENUM_CLASS_NAMES(enum_class_name) \
inline constexpr std::string_view _private::JOIN(enum_class_name, Names)[]

/**
 * ENUMERATORS.
 */

// Used to declare each value inside an enum class.
#define RAW_ENUM_ENUMERATOR(name) name,

// Used to declare each name value for each enum value.
#define ENUM_NAME_ENUMERATOR(name) #name,

// Used to generate a named constant for each enum value.
#define ENUM_CONSTANT_DECLARATION_ENUMERATOR(name) static const EnumType name;

// Used to generate the definition for each named constant for each enum value.
#define ENUM_CONSTANT_DEFINITION_ENUMERATOR(enum_class_name, name) \
 constexpr enum_class_name enum_class_name::name = enum_class_name::create(RawEnumType::name);

/**
 * The base class for Enums.
 */
#define ENUM_BASE(enum_class_name) \
 EnumBase<enum_class_name, _private::JOIN(enum_class_name, Raw), _private::JOIN(enum_class_name, Names)>

// The TokenBase class is simply a wrapper class for a enum type.
// It utilizes X-macros for ease of extension.
template <typename DerivedT, typename RawT, const std::string_view Names[]>
class EnumBase
{
  public:
    using RawEnumType    = RawT;
    using EnumType       = DerivedT;
    using UnderlyingType = std::underlying_type_t<RawT>;

    // Allow conversion to raw type.
    constexpr operator RawEnumType() const noexcept
    {
        return value;
    }

    [[nodiscard]] auto name() const -> const std::string_view
    {
        return Names[as_int()];
    }

    [[nodiscard]] static constexpr auto from_int(UnderlyingType v) -> EnumType
    {
      return create(static_cast<RawEnumType>(v));
    }

    [[nodiscard]] constexpr auto as_int() const noexcept -> UnderlyingType
    {
        return static_cast<UnderlyingType>(value);
    }

  protected:
    constexpr EnumBase() = default;

    static constexpr auto create(RawEnumType v) -> EnumType
    {
        EnumType r;
        r.value = v;
        return r;
    }

  private:
    RawEnumType value;
};

#endif
