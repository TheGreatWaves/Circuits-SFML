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

/**
 * NOTE: Main idea: parse the input (vm code) and translate the instructions
 *       into a vector of opcodes and values. 
 */

// Opcodes for the VM.
// NOTE: LABEL is not included.
enum class Opcode : uint16_t
{
 // Push operations.
 PUSH_CONSTANT,
 PUSH_STATIC,
 PUSH_TEMP,
 PUSH_POINTER,
 PUSH_LOCAL,
 PUSH_ARGUMENT,
 PUSH_THIS,
 PUSH_THAT,

 // Pop operations.
 POP_STATIC,
 POP_TEMP,
 POP_POINTER,
 POP_LOCAL,
 POP_ARGUMENT,
 POP_THIS,
 POP_THAT,

 ADD,
 AND,
 OR,
 SUB,
 NEG,
 NOT,
 EQ,
 GT,
 LT,
 LABEL, // This needs to be here. Quite troublesome.
 GOTO,  
 IF,
 CALL,
 FUNCTION,
 RETURN,
};

struct Chunk
{
 std::vector<uint16_t> code;

 Chunk()
 {
  code.reserve(100000);
 }


 auto emit(uint16_t value) -> void
 {
  code.push_back(value);
 }

 auto emit_instruction(Opcode instruction) -> void
 {
  emit(static_cast<uint16_t>(instruction));
 }
};


class EmulatedVMParser : public BaseParser<VMTokenType>
{
private:
 using TokenType = VMTokenType;

public:
 [[nodiscard]] explicit EmulatedVMParser(const std::string& file_path)
     : BaseParser<VMTokenType>(file_path)
 {
  setup_symbol_map();
 }

 [[nodiscard]] explicit EmulatedVMParser()
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
  // else if (match(TokenType::Pop))
  //  handle_pop();
  else if (match(TokenType::Add))
   handle_add();
  // else if (match(TokenType::And))
  //  handle_and();
  // else if (match(TokenType::Or))
  //  handle_or();
  // else if (match(TokenType::Sub))
  //  handle_sub();
  // else if (match(TokenType::Neg))
  //  handle_neg();
  // else if (match(TokenType::Not))
  //  handle_not();
  // else if (match(TokenType::Eq))
  //  handle_eq();
  // else if (match(TokenType::Gt))
  //  handle_gt();
  // else if (match(TokenType::Lt))
  //  handle_lt();
  // else if (match(TokenType::Label))
  //  handle_label();
  // else if (match(TokenType::Goto))
  //  handle_goto();
  // else if (match(TokenType::If))
  //  handle_if_goto();
  // else if (match(TokenType::Call))
  //  handle_call();
  // else if (match(TokenType::Function))
  //  handle_function();
  // else if (match(TokenType::Return))
  //  handle_return();
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
     consume(TokenType::Number, "Expected value after 'constant'");
     const std::uint16_t value = std::stoi(previous.lexeme);

     this->code.emit_instruction(Opcode::PUSH_CONSTANT);
     this->code.emit(value);
    }
    break; case TokenType::Static:
    {
     advance();
     consume(TokenType::Number, "Expected index after 'static'");
     const std::string index = previous.lexeme;
     const uint16_t address = get_symbol("STATIC_" + index);

     this->code.emit_instruction(Opcode::PUSH_STATIC);
     this->code.emit(address);
    }
    break; case TokenType::Temp:
    {
     advance();
     consume(TokenType::Number, "Expected index after 'temp'");

     const std::string index_string = previous.lexeme;
     const uint32_t offset = std::stoi(index_string);

     if (offset > 7) report_error("Temp index out of range: " + index_string);
     this->code.emit_instruction(Opcode::PUSH_TEMP);
     this->code.emit(offset);
    }
    break; case TokenType::Pointer:
    {
     advance();
     consume(TokenType::Number, "Expected index after 'pointer'");
     const std::string index = previous.lexeme;

     if (index != "1" && index != "0") report_error("Invalid pointer for push");

     const uint16_t addr = (index == "1") 
                               ? (get_symbol("THAT")) 
                               : (get_symbol("THIS"));
     this->code.emit_instruction(Opcode::PUSH_POINTER);
     this->code.emit(addr);
    }
    break; case TokenType::Local:    { this->code.emit_instruction(Opcode::PUSH_LOCAL); write_push_segment(); }
    break; case TokenType::Argument: { this->code.emit_instruction(Opcode::PUSH_ARGUMENT); write_push_segment(); }
    break; case TokenType::This:     { this->code.emit_instruction(Opcode::PUSH_THIS); write_push_segment(); }
    break; case TokenType::That:     { this->code.emit_instruction(Opcode::PUSH_THAT); write_push_segment(); }
    break; default: { report_error("Unexpected segment found in push statement"); }
  }
 }

 auto write_push_segment() -> void
 {
  advance();
  const std::string segment_name = previous.lexeme;
  consume(TokenType::Number, "Expected index after '" + segment_name + "'");

  // Offset index (from segment).
  const uint16_t offset = std::stoi(previous.lexeme);
  this->code.emit(offset);
 }

 auto handle_pop() -> void 
 {
 }

 auto handle_add() -> void 
 {
  this->code.emit_instruction(Opcode::ADD);
 }

 auto handle_and() -> void 
 {
  this->code.emit_instruction(Opcode::AND);
 }

 auto handle_or() -> void 
 {
  this->code.emit_instruction(Opcode::OR);
 }

 auto handle_sub() -> void 
 {
  this->code.emit_instruction(Opcode::SUB);
 }

 auto handle_neg() -> void
 {
  this->code.emit_instruction(Opcode::NEG);
 }

 auto handle_not() -> void
 {
  this->code.emit_instruction(Opcode::NOT);
 }

 auto handle_eq() -> void
 {
  this->code.emit_instruction(Opcode::EQ);
 }

 auto handle_gt() -> void
 {
  this->code.emit_instruction(Opcode::GT);
 }

 auto handle_lt() -> void
 {
  this->code.emit_instruction(Opcode::LT);
 }

 auto handle_label() -> void
 {
  this->code.emit_instruction(Opcode::LABEL);

  consume(TokenType::Identifier, "Expected label name");
  const std::string label_name = previous.lexeme;
  const uint16_t label_value = get_symbol(label_name);

  this->code.emit(label_value);
 }

 
 auto handle_goto() -> void
 {
  consume(TokenType::Identifier, "Expected label name");
  const std::string label_name = previous.lexeme;
  this->code.emit_instruction(Opcode::GOTO);
  this->code.emit(get_symbol(label_name));
 }

 auto handle_if_goto() -> void
 {
  consume(TokenType::Dash, "Expected '-' after if");
  consume(TokenType::Goto, "Expected 'goto' after '-'");
  consume(TokenType::Identifier, "Expected label name");
  const std::string label_name = previous.lexeme;
  this->code.emit_instruction(Opcode::IF);
  this->code.emit(get_symbol(label_name));
 }

 auto handle_call() -> void
 {
  consume(TokenType::Identifier, "Expected file name");
  const std::string file_name = previous.lexeme;
  consume(TokenType::Dot, "Expected function name");
  advance();
  const std::string function_name = previous.lexeme;
  consume(TokenType::Number, "Expected function args count");
  const std::string n_args = previous.lexeme;

  call(file_name, function_name, n_args);
 }

 auto call(const std::string& file_name, const std::string& function_name, const std::string& n_args) -> void
 {
 }

 auto handle_function() -> void
 {
 }

 auto patch(const uint16_t offset, const uint16_t value) -> void
 {
 }

 auto handle_return() -> void
 {
  const uint16_t SP = get_symbol("SP");
  const uint16_t LCL = get_symbol("LCL");
  const uint16_t ARG = get_symbol("ARG");
  const uint16_t THIS = get_symbol("THIS");
  const uint16_t THAT = get_symbol("THAT");

  // Retrieve address of the frame's end.
  const uint16_t end_frame_address = this->computer.at(LCL);

  // Retrieve the return address.
  const uint16_t return_address = this->computer.at(end_frame_address - 5);

  // Replace the first argument passed into the function 
  // with the return value from the function.
  const uint16_t function_result_value = this->computer.pop_stack();
  const uint16_t arg_address = this->computer.at(ARG);
  this->computer.write_at(arg_address, function_result_value);

  // Set the stack pointer to 1 below ARG pointer.
  // Which we previously replaced with the return value.
  this->computer.write_at(SP, arg_address + 1);

  // Restore all pointers.
  this->computer.write_at(THAT, end_frame_address - 1);
  this->computer.write_at(THIS, end_frame_address - 2);
  this->computer.write_at(ARG, end_frame_address - 3);
  this->computer.write_at(LCL, end_frame_address - 4);

  // Jump to the return address.
  this->computer.jump(return_address);
 }

 /**
  * Retrieves the ID of the given symbol.
  * Assigns a new ID if not given and a corresponding entry in the value vector is generated.
  */
 auto get_symbol(const std::string& symbol) -> uint16_t
 {
  if (symbol_map.find(symbol) == symbol_map.end()) 
  {
   // Index increases monotonically, values are packed.
   symbol_map[symbol] = symbol_map.size();
   value_vector.push_back(0);
  }
  return symbol_map[symbol];
 }
private:
 auto setup_symbol_map() -> void
 {
  // TODO: Initialize the actual value of the symbols.
  // eg. "SP" == SYMBOL(0) == VALUE(0)

  get_symbol("SP");   // 0
  get_symbol("LCL");  // 1
  get_symbol("ARG");  // 2
  get_symbol("THIS"); // 3
  get_symbol("THAT"); // 4

  for (std::size_t index {0}; index < 16; index++)
  {
   std::stringstream ss {};
   ss << "R" << index;
   get_symbol(ss.str());
  }
 }

 auto count() -> uint16_t
 {
  return m_count++;
 }

private:
 emulator::Computer                        computer            {};
 Chunk                                     code                {};

 // The symbol map is used to retrieve the numerical ID corressponding to a given symbol.
 std::unordered_map<std::string, uint16_t> symbol_map          {};
 std::vector<uint16_t>                     value_vector        {};
 uint16_t                                  loc                 {0};
 std::uint16_t                             m_count             {};
};

#endif // EMULATED_VM_HPP
