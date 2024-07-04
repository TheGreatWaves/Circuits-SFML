#include <limits>

#include "computer.hpp"
#include "../lang/vm/vm.hpp"

auto main() -> int
{
 VMTranslator translator("in.vm");

 if (!translator.parse())
 {
  std::cout << "Failed to parse" << '\n';
  return 1;
 }

 translator.print();

 const auto instructions = translator.to_instructions();

 emulator::Computer computer {};

 computer.load_instructions(instructions);
 const auto loc = translator.loc();
 std::cout << "#instructions: " << loc << '\n';
 computer.process(5000);


 std::cout << "\n=== State ===\n";
 computer.print_state();

 return 0;
}


