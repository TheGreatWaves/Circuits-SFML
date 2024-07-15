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


#include <chrono>
#include <future>
#include <sstream>

#include "devices/screen.hpp"
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
 : m_window(sf::VideoMode(512, 256), "Hack Computer")
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

  translator.set_static_count(m_static_count);

  if (!translator.parse()) return false;

  m_buffer << translator.build();

  m_static_count = translator.get_static_count();

  return true;
 }

 auto out() -> void
 {
  std::cout << m_buffer.str() << '\n';
 }

 auto compile() -> bool
 {
  VMTranslator translator {};
  translator.set_source(m_buffer.str());

  if (!translator.parse())
  {
   std::cout << "Failed to compile" << '\n';
   return false;
  }

  const auto instructions = translator.to_instructions();

  m_computer.load_instructions(instructions);


  // const auto loc = translator.loc();
  // std::cout << "#instructions: " << loc << '\n';
  // std::cout << "\n=== State ===\n";
  // computer.print_state();

  return true;
}

auto run() -> void
{
 const auto cycle_count = 100000000;
 auto thread = std::async(std::launch::async, [this]{ while (true) { 
   const auto start = std::chrono::high_resolution_clock::now();
   m_computer.process(cycle_count);
   const auto end = std::chrono::high_resolution_clock::now();
   const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
   std::cout << "#Cycles: " << cycle_count << ", " << duration.count() << " nacnoseconds" << '\n';
 }});
 while (m_window.isOpen())
 {
  sf::Event event;
  while (m_window.pollEvent(event))
  {
   if (event.type == sf::Event::Closed)
       m_window.close();
  }

  m_window.clear();
  m_screen.draw(m_window, m_computer.m_ram);
  m_window.display();  
 }
 thread.get();
}

private:
 Screen m_screen{};
 emulator::Computer m_computer {};
 sf::RenderWindow m_window {};
 std::stringstream m_buffer {};
 std::size_t m_static_count {};
};

#endif /* COMPILATION_CONTEXT_HPP */

