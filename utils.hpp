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
template<typename ... Args> auto log(Args&& ... args)->void{(std::cout<<...<<std::forward<Args>(args));} 

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

#define BLOCK "======"

#endif /* UTILS */
