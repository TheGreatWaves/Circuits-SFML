#include <limits>

#include "computer.hpp"
#include "../lang/jack/jack.hpp"
#include "../lang/vm/vm.hpp"
#include "compilation_context.hpp"

auto main() -> int
{
 // JackParser parser("in.jack");

 // parser.m_writer.write_call("Main.main", "0");
 // parser.m_writer.write_label("END");
 // parser.m_writer.write_goto("END");

 // if (!parser.parse())
 // {
 //  return 1;
 // }

 // auto vm_code = parser.build();
 // std::cout << vm_code << '\n';

 // VMTranslator translator {};

 // translator.set_source(vm_code);

 // if (!translator.parse())
 // {
 //  std::cout << "Failed to parse" << '\n';
 //  return 1;
 // }

 // // translator.print();

 // const auto instructions = translator.to_instructions();

 // emulator::Computer computer {};

 // computer.load_instructions(instructions);
 // const auto loc = translator.loc();
 // std::cout << "#instructions: " << loc << '\n';
 // computer.process(15000);


 // std::cout << "\n=== State ===\n";
 // computer.print_state();

 // return 0;

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
 context.compile_and_run();

 return 0;
}


