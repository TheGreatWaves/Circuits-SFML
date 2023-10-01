#pragma once
#ifndef UTILS
#define UTILS

#include <algorithm>
#include <string_view>
#include <cctype>

inline std::string make_lower(std::string_view str)
{
  std::string input = std::string(str);
  auto lower = [](unsigned char c){ return std::tolower(c); };
  std::transform(input.begin(), input.end(), input.begin(), lower);
  return input;
}

#endif /* UTILS */
