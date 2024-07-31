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
#ifndef EMULATED_VM_HPP
#define EMULATED_VM_HPP

#include <iterator>
#include <string>
#include <sstream>
#include <string_view>
#include <filesystem>
namespace fs = std::filesystem;

#include "../core/parser_base.hpp"
#include "token_vm.hpp"
#include "../../emulator/computer.hpp"
#include "../assembler/assembler.hpp"

class EmulatedVM : public BaseParser<VMTokenType>
{
private:
 using TokenType = VMTokenType;

public:
 [[nodiscard]] explicit EmulatedVM(const std::string& file_path)
     : BaseParser<VMTokenType>(file_path)
 {
  setup_symbol_map();
 }

 [[nodiscard]] explicit EmulatedVM()
     : BaseParser<VMTokenType>()
 {
  setup_symbol_map();
 }

 auto set_source(const std::string& input) -> void
 {
  scanner.set_source(input);
 }

 /**
  * Parse the source code and excute the instructions.
  */
 [[nodiscard]] auto parse() noexcept -> bool
 {
  advance();

  while (!match(TokenType::EndOfFile))
  {
   instruction();
   loc++;
  }

  return !this->has_error;
 }

 auto instruction() -> void
 {
  if (match(TokenType::Push))
   handle_push();
  else if (match(TokenType::Pop))
   handle_pop();
  else if (match(TokenType::Add))
   handle_add();
  else if (match(TokenType::And))
   handle_and();
  else if (match(TokenType::Or))
   handle_or();
  else if (match(TokenType::Sub))
   handle_sub();
  else if (match(TokenType::Neg))
   handle_neg();
  else if (match(TokenType::Not))
   handle_not();
  else if (match(TokenType::Eq))
   handle_eq();
  else if (match(TokenType::Gt))
   handle_gt();
  else if (match(TokenType::Lt))
   handle_lt();
  else if (match(TokenType::Label))
   handle_label();
  else if (match(TokenType::Goto))
   handle_goto();
  else if (match(TokenType::If))
   handle_if_goto();
  else if (match(TokenType::Call))
   handle_call();
  else if (match(TokenType::Function))
   handle_function();
  else if (match(TokenType::Return))
   handle_return();
  else
  {
   const std::string current = this->current.lexeme;
   report_error("Invalid token: " + current);
  }

  // TODO: Handle error.
  if (this->has_error) 
  {
   report_error("Something went wrong...");
   advance();
  }
 }
 
 auto handle_push() -> void 
 {
  switch (this->current.type)
  {
    break; case TokenType::Constant: 
    {
     advance();
     consume(TokenType::Number, "Expected index after 'constant'");
     const std::uint16_t index = std::stoi(previous.lexeme);
     this->computer.stack_push_constant(index);
    }
    break; case TokenType::Static:
    {
     advance();
     consume(TokenType::Number, "Expected index after 'static'");
     const std::string index = previous.lexeme;
     const uint16_t symbol_address = get_symbol("STATIC_" + index);
     this->computer.stack_push_value_at(symbol_address);
    }
    break; case TokenType::Temp:
    {
     advance();
     consume(TokenType::Number, "Expected index after 'temp'");
     const std::string index_string = previous.lexeme;
     const uint32_t offset = std::stoi(index_string);
     if (offset > 7) report_error("Temp index out of range: " + index_string);
     this->computer.stack_push_value_at(offset + 5);
    }
    break; case TokenType::Pointer:
    {
     advance();
     consume(TokenType::Number, "Expected index after 'pointer'");
     const std::string index = previous.lexeme;

     if (index != "1" && index != "0") report_error("Invalid pointer for push");

     const uint16_t addr = (index == "1") 
                               ? (symbol_map["THAT"]) 
                               : (symbol_map["THIS"]);
     this->computer.stack_push_value_at(addr);
    }
    break; case TokenType::Local:    write_push_segment("LCL");
    break; case TokenType::Argument: write_push_segment("ARG");
    break; case TokenType::This:     write_push_segment("THIS");
    break; case TokenType::That:     write_push_segment("THAT");
    break; default: { report_error("Unexpected segment found in push statement"); }
  }
 }

 auto write_push_segment(const std::string& segment) -> void
 {
  advance();
  const std::string segment_name = previous.lexeme;
  consume(TokenType::Number, "Expected index after '" + segment_name + "'");

  // Offset index (from segment).
  const uint16_t offset = std::stoi(previous.lexeme);

  // Retrieve the value of the address stored inside the segment slot.
  const uint16_t segment_slot_addr = symbol_map[segment];
  const uint16_t segment_addr = this->computer.at(segment_slot_addr);

  // Get the address + offset.
  const uint16_t target_address = segment_addr + offset;

  // Push the value onto the stack.
  this->computer.stack_push_value_at(target_address);
 }

 auto handle_pop() -> void 
 {
 }

 auto handle_add() -> void 
 {
  const uint16_t b = this->computer.pop_stack();
  const uint16_t a = this->computer.peek();
  this->computer.replace_top(a + b);
 }

 auto handle_and() -> void 
 {
  const uint16_t b = this->computer.pop_stack();
  const uint16_t a = this->computer.peek();
  this->computer.replace_top(a & b);
 }

 auto handle_or() -> void 
 {
  const uint16_t b = this->computer.pop_stack();
  const uint16_t a = this->computer.peek();
  this->computer.replace_top(a | b);
 }

 auto handle_sub() -> void 
 {
  const uint16_t b = this->computer.pop_stack();
  const uint16_t a = this->computer.peek();
  this->computer.replace_top(a - b);
 }

 auto handle_neg() -> void
 {
  const uint16_t value = this->computer.peek();
  this->computer.replace_top(-value);
 }

 auto handle_not() -> void
 {
  const uint16_t value = this->computer.peek();
  this->computer.replace_top((value > 0) ? -1 : 0);
 }

 auto handle_eq() -> void
 {
  const uint16_t b = this->computer.pop_stack();
  const uint16_t a = this->computer.peek();
  this->computer.replace_top((a == b) ? -1 : 0);
 }

 auto handle_gt() -> void
 {
  const uint16_t b = this->computer.pop_stack();
  const uint16_t a = this->computer.peek();
  this->computer.replace_top((a > b) ? -1 : 0);
 }

 auto handle_lt() -> void
 {
  const uint16_t b = this->computer.pop_stack();
  const uint16_t a = this->computer.peek();
  this->computer.replace_top((a < b) ? -1 : 0);
 }

 auto handle_label() -> void
 {
  consume(TokenType::Identifier, "Expected label name");
  const std::string label_name = previous.lexeme;
  this->label_map[label_name] = loc;
 }
 
 auto handle_goto() -> void
 {
  consume(TokenType::Identifier, "Expected label name");
  const std::string label_name = previous.lexeme;
  this->computer.jump(label_map[label_name]);
 }

 auto handle_if_goto() -> void
 {
  consume(TokenType::Dash, "Expected '-' after if");
  consume(TokenType::Goto, "Expected 'goto' after '-'");
  consume(TokenType::Identifier, "Expected label name");
  const std::string label_name = previous.lexeme;
  const bool cond = this->computer.pop_stack() > 0;
  this->computer.jump_if(cond, label_map[label_name]);
 }

 auto handle_call() -> void
 {
 }

 auto handle_function() -> void
 {
 }

 auto handle_return() -> void
 {
 }

 auto get_symbol(const std::string& symbol) -> uint16_t
 {
  if (symbol_map.find(symbol) == symbol_map.end()) 
   symbol_map[symbol] = next_variable_index++;
  return symbol_map[symbol];
 }
private:
 auto setup_symbol_map() -> void
 {
  symbol_map["SP"]   = 0;
  symbol_map["LCL"]  = 1;
  symbol_map["ARG"]  = 2;
  symbol_map["THIS"] = 3;
  symbol_map["THAT"] = 4;

  for (std::size_t index {0}; index < 16; index++)
  {
   std::stringstream ss {};
   ss << "R" << index;
   symbol_map[ss.str()] = index;
  }
 }

private:
 emulator::Computer computer                          {};
 std::unordered_map<std::string, uint16_t> symbol_map {};
 std::unordered_map<std::string, uint16_t> label_map {};
 uint16_t next_variable_index                         {16};
 uint16_t loc {0};
};

#endif // EMULATED_VM_HPP
