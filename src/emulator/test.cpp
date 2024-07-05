#include <limits>

#include "computer.hpp"
#include "../lang/jack/jack.hpp"

auto main() -> int
{
 JackParser translator("in.vm");

 if (!translator.parse())
 {
  return 1;
 }

 translator.output();

 return 0;
}


