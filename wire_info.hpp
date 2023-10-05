#pragma once
#ifndef WIRE_INFO
#define WIRE_INFO

#include <vector>

struct WireInfo
{ 
  std::size_t src; 
  std::size_t dest; 
};

using WireConstructionInfo = std::vector<WireInfo>;

#endif /* WIRE_INFO */
