/** 
 * MIT License
 * 
 * Copyright (c) 2023 Ochawin A.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once
#ifndef COMPILATION_CONTEXT_HPP
#define COMPILATION_CONTEXT_HPP

#include <sstream>

#include "computer.hpp"
#include "../lang/jack/jack.hpp"
#include "../lang/vm/vm.hpp"

/**
 * Buffer for instructions. Allows for compilation of files which adds onto the buffer.
 */

struct CompilationContext
{
public:
 explicit CompilationContext()
 {
  bootstrap();
 };

 auto bootstrap() -> void 
 {
  m_buffer << "push constant 0" << '\n';   // Stack Top = 0
  m_buffer << "pop pointer 1" << '\n';     // that = &0
  m_buffer << "push constant 256" << '\n'; // Stack Top = 256
  m_buffer << "pop that 0" << '\n';        // *(&0) = 256
  m_buffer << "call Sys.init 0" << '\n';
  // m_buffer << "call Main.main 0" << '\n';
  // m_buffer << "label END" << '\n';
  // m_buffer << "goto END" << '\n';
 }

 [[nodiscard]] auto add_source(const std::string& path) -> bool
 {
  JackParser translator(path);

  if (!translator.parse()) return false;

  m_buffer << translator.build();

  return true;
 }

 auto out() -> void
 {
  std::cout << m_buffer.str() << '\n';
 }

 auto compile_and_run() -> bool
 {
  VMTranslator translator {};
  translator.set_source(m_buffer.str());

  if (!translator.parse())
  {
   std::cout << "Failed to compile" << '\n';
   return false;
  }

  const auto instructions = translator.to_instructions();

  emulator::Computer computer {};

  computer.load_instructions(instructions);
  computer.process(100000);


  const auto loc = translator.loc();
  std::cout << "#instructions: " << loc << '\n';
  std::cout << "\n=== State ===\n";
  computer.print_state();

  return true;
}

private:
 std::stringstream m_buffer;
};

#endif /* COMPILATION_CONTEXT_HPP */

