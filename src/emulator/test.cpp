#include <limits>

#include "computer.hpp"
#include "../lang/jack/jack.hpp"
#include "../lang/vm/vm.hpp"
#include "compilation_context.hpp"

auto main() -> int
{
 CompilationContext context {};

 if (!context.add_source("os/memory.jack"))
 {
  std::cout << "Failed to add source: os/memory.jack" << '\n'; 
 }

 if (!context.add_source("os/array.jack"))
 {
  std::cout << "Failed to add source: os/array.jack" << '\n'; 
 }

 if (!context.add_source("os/system.jack"))
 {
  std::cout << "Failed to add source: os/system.jack" << '\n'; 
 }

 if (!context.add_source("os/math.jack"))
 {
  std::cout << "Failed to add source: os/math.jack" << '\n'; 
 }

 if (!context.add_source("os/screen.jack"))
 {
  std::cout << "Failed to add source: os/screen.jack" << '\n'; 
 }

 if (!context.add_source("main.jack"))
 {
  std::cout << "Failed to add source: main.jack" << '\n'; 
 }

 context.out();
 context.compile();
 context.run();

 return 0;
}


