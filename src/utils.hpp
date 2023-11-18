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
#ifndef UTILS
#define UTILS

#include <algorithm>
#include <string_view>
#include <cctype>
#include <iostream>

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

#define BLOCK "======"

#endif /* UTILS */
