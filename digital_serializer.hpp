#pragma once
#ifndef D_SERIALIZER
#define D_SERIALIZER

#include <vector>
#include <fstream>

struct Gate
{
  std::vector<bool> input_ports;
  std::vector<bool> output_ports;

  explicit Gate(std::size_t ic, std::size_t oc)
  {
    input_ports.resize(ic);
    output_ports.resize(oc);
  }

  void simulate(const std::vector<bool>& in)
  {
    
  }

  void serialize_gate()
  {
    
  }
};

#endif /* D_SERIALIZER */