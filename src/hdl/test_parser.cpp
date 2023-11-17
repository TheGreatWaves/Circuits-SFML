#include "parser.hpp"

auto main() -> int {
 auto parser = hdl::HDLParser("Alex.hdl");

 if (!parser.parse())
 {
  std::cerr << "Something went very wrong!\n";
 }
 else
 {
  parser.result().build();
 }
}