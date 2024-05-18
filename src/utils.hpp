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
#ifndef UTILS_H
#define UTILS_H

#include <type_traits>
#include <algorithm>
#include <string_view>
#include <cctype>
#include <iostream>
#include "pin.hpp"

inline std::string make_lower(std::string_view str)
{
  std::string input = std::string(str);
  auto lower = [](unsigned char c){ return std::tolower(c); };
  std::transform(input.begin(), input.end(), input.begin(), lower);
  return input;
}

/**
 * Logging related functions.
 */
template<typename ... Args> auto log(Args&& ... args)->void{(std::cout<<...<<std::forward<Args>(args)) << '\n';} 
template<typename ... Args> auto print(Args&& ... args)->void{(std::cout<<...<<std::forward<Args>(args));} 

inline void desc(std::string_view name, std::string_view brief)
{
	std::cout << name << ": " << brief << '\n';
}

inline void newline()
{
	std::cout << '\n';
}

inline void info(std::string_view dump)
{
	std::cout << dump << '\n';
}

inline void error(std::string_view dump)
{
	std::cout << "Error: " << dump << '\n';
}

template <typename ReturnType, typename DataType>
inline std::enable_if_t<std::is_unsigned_v<ReturnType>, ReturnType> 
bitvec_to_uint(const std::vector<DataType>& vec, const std::function<bool(const DataType&)> f, std::size_t start, std::size_t end)
{
	ReturnType val {0};

	for (; start < end; start++)
	{
		val <<= 1;
		val |= (f(vec[start])) ? 1 : 0;
	}

	return val;
}

template <typename ReturnType = std::size_t>
inline std::enable_if_t<std::is_unsigned_v<ReturnType>, ReturnType> 
pinvec_to_uint(const std::vector<Pin>& vec, std::size_t start, std::size_t end)
{
	auto pin_is_active = [](const Pin& p) { return p.is_active(); };
	return bitvec_to_uint<ReturnType, Pin>(vec, std::move(pin_is_active), start, end);
}

template <typename T, typename VT>
inline std::enable_if_t<std::is_unsigned_v<T>> 
set_bitvec(T value, std::vector<VT>& target, std::function<void(VT&, bool)> f, std::size_t start, std::size_t end)
{
	T size = end - start;
  for (T offset = 0; offset < size; offset++)
  {
		auto bit_on = (((value >> ((size-1) - offset) ) & 1) == 1);
		f(target[start + offset], bit_on);
  }
}

template <typename T>
inline std::enable_if_t<std::is_unsigned_v<T>> 
set_pinvec(T value, std::vector<Pin>& target, std::size_t start, std::size_t end)
{
	const auto f = [](Pin& pin, bool on){ pin.state = on ? PinState::ACTIVE : PinState::INACTIVE; };
	set_bitvec<T, Pin>(value, target, std::move(f), start, end);
}


#define BLOCK "======"

#endif /* UTILS_H */
