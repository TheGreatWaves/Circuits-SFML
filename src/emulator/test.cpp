#include <limits>

#include "computer.hpp"
#include "../lang/jack/jack.hpp"
#include "../lang/vm/vm.hpp"

auto main() -> int
{
 JackParser parser("in.jack");

 parser.m_writer.write_call("Main.main", "0");
 parser.m_writer.write_label("END");
 parser.m_writer.write_goto("END");

 if (!parser.parse())
 {
  return 1;
 }

 auto vm_code = parser.build();
 std::cout << vm_code << '\n';

 VMTranslator translator {};

 translator.set_source(vm_code);

 if (!translator.parse())
 {
  std::cout << "Failed to parse" << '\n';
  return 1;
 }

 // translator.print();

 const auto instructions = translator.to_instructions();

 emulator::Computer computer {};

 computer.load_instructions(instructions);
 const auto loc = translator.loc();
 std::cout << "#instructions: " << loc << '\n';
 computer.process(1000);


 std::cout << "\n=== State ===\n";
 computer.print_state();

 return 0;
}


