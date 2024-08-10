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
#include <iomanip>


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
 PUSH_LABEL,
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

 auto dump(std::ostream& os = std::cout) -> void
 {
  for (std::size_t i {0}; i < code.size(); i++)
  {
   std::cout << std::setw(4) << std::right << i << " | ";

   const auto instruction = code[i];


   std::cout << std::setw(15) << std::left;

   switch (static_cast<Opcode>(instruction))
   {
       break; case Opcode::PUSH_CONSTANT:
       {
        std::cout << "PUSH_CONSTANT " << code[++i];
       }
       break; case Opcode::PUSH_LABEL:
       {
        std::cout << "PUSH_LABEL " << code[++i];
       }
       break; case Opcode::PUSH_STATIC:
       {
        std::cout << "PUSH_STATIC " << code[++i];
       }
       break; case Opcode::PUSH_TEMP:
       {
        std::cout << "PUSH_TEMP " << code[++i];
       }
       break; case Opcode::PUSH_POINTER:
       {
        std::cout << "PUSH_POINTER " << code[++i];
       }
       break; case Opcode::PUSH_LOCAL:
       {
        std::cout << "PUSH_LOCAL " << code[++i];
       }
       break; case Opcode::PUSH_ARGUMENT:
       {
        std::cout << "PUSH_ARGUMENT " << code[++i];
       }
       break; case Opcode::PUSH_THIS:
       {
        std::cout << "PUSH_THIS " << code[++i];
       }
       break; case Opcode::PUSH_THAT:
       {
        std::cout << "PUSH_THAT " << code[++i];
       }
       break; case Opcode::POP_STATIC:
       {
        std::cout << "POP_STATIC " << code[++i];
       }
       break; case Opcode::POP_TEMP:
       {
        std::cout << "POP_TEMP " << code[++i];
       }
       break; case Opcode::POP_POINTER:
       {
        std::cout << "POP_POINTER " << code[++i];
       }
       break; case Opcode::POP_LOCAL:
       {
        std::cout << "POP_LOCAL " << code[++i];
       }
       break; case Opcode::POP_ARGUMENT:
       {
        std::cout << "POP_ARGUMENT " << code[++i];
       }
       break; case Opcode::POP_THIS:
       {
        std::cout << "POP_THIS " << code[++i];
       }
       break; case Opcode::POP_THAT:
       {
        std::cout << "POP_THAT " << code[++i];
       }
       break; case Opcode::ADD:
       {
        std::cout << "ADD";
       }
       break; case Opcode::AND:
       {
        std::cout << "AND";
       }
       break; case Opcode::OR:
       {
        std::cout << "OR";
       }
       break; case Opcode::SUB:
       {
        std::cout << "SUB";
       }
       break; case Opcode::NEG:
       {
        std::cout << "NEG";
       }
       break; case Opcode::NOT:
       {
        std::cout << "NOT";
       }
       break; case Opcode::EQ:
       {
        std::cout << "EQ";
       }
       break; case Opcode::GT:
       {
        std::cout << "GT";
       }
       break; case Opcode::LT:
       {
        std::cout << "LT";
       }
       break; case Opcode::LABEL:
       {
        std::cout << "LABEL " << "[" << code[++i] << "]";
       }
       break; case Opcode::GOTO:
       {
        std::cout << "GOTO " << "[" << code[++i] << "]";
       }
       break; case Opcode::IF:
       {
        std::cout << "IF " << "[" << code[++i] << "]";
       }
       break; case Opcode::CALL:
       {
        std::cout << "CALL " << "[" << code[++i] << "] " << code[++i];
       }
       break; case Opcode::FUNCTION:
       {
        std::cout << "FUNCTION " << "[" << code[++i] << "] " << code[++i];
       }
       break; case Opcode::RETURN:
       {
        std::cout << "RETURN";
       }
       break; default: {}
   }

   std::cout << '\n';
  }
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
  const auto segment_type = this->current.type;
  switch (segment_type)
  {
    break; case TokenType::Static:
    {
     advance();
     consume(TokenType::Number, "Expected index after 'static'");
     const std::string index_str = previous.lexeme;
     const uint16_t index = std::stoi(index_str);

     this->code.emit_instruction(Opcode::POP_STATIC);
     this->code.emit(index);
    }
    break; case TokenType::Temp:
    {
     advance();
     consume(TokenType::Number, "Expected index after 'temp'");
     const std::string index_str = previous.lexeme;
     const uint32_t index = std::stoi(index_str);

     if (index > 7) report_error("Temp index out of range: " + index_str);

     this->code.emit_instruction(Opcode::POP_TEMP);
     this->code.emit(index);
    }
    break; case TokenType::Pointer:
    {
     advance();
     consume(TokenType::Number, "Expected index after 'pointer'");
     const std::string index_str = previous.lexeme;
     const uint16_t index = std::stoi(index_str);

     if (index_str != "1" && index_str != "0") report_error("Invalid pointer for pop");

     this->code.emit_instruction(Opcode::POP_POINTER);
     this->code.emit(index);
    }
    break; case TokenType::Local:    { this->code.emit_instruction(Opcode::POP_LOCAL); write_pop_segment("LCL"); }
    break; case TokenType::Argument: { this->code.emit_instruction(Opcode::POP_ARGUMENT); write_pop_segment("ARG"); }
    break; case TokenType::This:     { this->code.emit_instruction(Opcode::POP_THIS); write_pop_segment("THIS"); }
    break; case TokenType::That:     { this->code.emit_instruction(Opcode::POP_THAT); write_pop_segment("THAT"); }
    break; default: { report_error("Unexpected segment found in pop statement: " + std::string()); }
  } 
 }

 auto write_pop_segment(std::string_view segment) -> void
 {
     advance();
     const std::string segment_name = previous.lexeme;
     consume(TokenType::Number, "Expected index after '" + segment_name + "'");
     const std::string index_str = previous.lexeme;
     const uint16_t index = std::stoi(index_str);

     this->code.emit(index);
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
  consume(TokenType::Identifier, "Expected label name");
  const std::string label_name = previous.lexeme;
  const uint16_t label_value = get_symbol(label_name);

  this->code.emit_instruction(Opcode::LABEL);
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
  const std::string n_args_str = previous.lexeme;
  const uint16_t n_args = std::stoi(n_args_str);
  const std::string function_path = file_name + "." + function_name;

  this->code.emit_instruction(Opcode::CALL);
  this->code.emit(get_symbol(function_path));
  this->code.emit(n_args);
 }

 auto handle_function() -> void
 {
  consume(TokenType::Identifier, "Expected file name");
  const std::string file_name = previous.lexeme;
  consume(TokenType::Dot, "Expected function name");
  advance();
  const std::string function_name = previous.lexeme;
  consume(TokenType::Number, "Expected variable count");
  const std::string n_args_str = previous.lexeme;
  const uint16_t n_args = std::stoi(n_args_str);
  const std::string function_path = file_name + "." + function_name;

  this->code.emit_instruction(Opcode::FUNCTION);
  this->code.emit(get_symbol(function_path));
  this->code.emit(n_args);
 }

 auto handle_return() -> void
 {
  this->code.emit_instruction(Opcode::RETURN);
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

 /**
  * Assign labels with line number and symbols with an incrementing value from 16 onwards.
  */
 auto generate_symbols_and_labels() -> void
 {
  for (std::size_t line {0}; line < code.code.size(); line++)
  {
   const uint16_t instruction = code.code[line];
   switch (static_cast<Opcode>(instruction))
   {
     break; case Opcode::LABEL:
     {
      const uint16_t symbol = code.code[line+1];
      // Offset 2: [LABEL], [SYMBOL], [NEXT INSTRUCTION]
      value_vector[symbol] = line + 2;
      line += 1;
     }
     break; case Opcode::CALL:
     {
      line += 2;
     }
     break; case Opcode::FUNCTION:
     {
      const uint16_t symbol = code.code[line+1];
      // Offset 3: [FUNCTION], [SYMBOL], [ARG COUNT], [NEXT INSTRUCTION]
      value_vector[symbol] = line + 3;
      line += 2;
     }
     break; case Opcode::PUSH_CONSTANT:
            case Opcode::PUSH_LABEL:
            case Opcode::PUSH_STATIC:
            case Opcode::PUSH_TEMP:
            case Opcode::PUSH_POINTER:
            case Opcode::PUSH_LOCAL:
            case Opcode::PUSH_ARGUMENT:
            case Opcode::PUSH_THIS:
            case Opcode::PUSH_THAT:
            case Opcode::POP_STATIC:
            case Opcode::POP_TEMP:
            case Opcode::POP_POINTER:
            case Opcode::POP_LOCAL:
            case Opcode::POP_ARGUMENT:
            case Opcode::POP_THIS:
            case Opcode::POP_THAT:
            case Opcode::GOTO:
            case Opcode::IF:
     {
      line += 1;
     }
     break; case Opcode::ADD:
     break; case Opcode::AND:
     break; case Opcode::OR:
     break; case Opcode::SUB:
     break; case Opcode::NEG:
     break; case Opcode::NOT:
     break; case Opcode::EQ:
     break; case Opcode::GT:
     break; case Opcode::LT:
     break; case Opcode::RETURN:
     break; default: {}
   }
  }

  std::size_t counter {16};
  for (std::size_t i {15}; i < value_vector.size(); i++)
  {
   if (value_vector[i] == 0)
    value_vector[i] = counter++;
  }
 }

 auto dump_value_map() -> void
 {
  for (const auto& [key, value] : symbol_map)
  {
   std::cout << "[" <<  std::setw(3) << std::right << value << "] " << std::setw(23) << std::left << key << value_vector[value] << '\n';
  }
 }

private:
 auto setup_symbol_map() -> void
 {
  // TODO: Initialize the actual value of the symbols.
  // eg. "SP" == SYMBOL(0) == VALUE(0)

  value_vector[get_symbol("SP")] = 0;   // 0
  value_vector[get_symbol("LCL")] = 1;  // 1
  value_vector[get_symbol("ARG")] = 2;  // 2
  value_vector[get_symbol("THIS")] = 3; // 3
  value_vector[get_symbol("THAT")] = 4; // 4

  for (std::size_t index {0}; index < 16; index++)
  {
   std::stringstream ss {};
   ss << "R" << index;
   value_vector[get_symbol(ss.str())] = index;
  }
 }

 auto count() -> uint16_t
 {
  return m_count++;
 }

public:
 Chunk                                     code                {};
private:
 emulator::Computer                        computer            {};

 // The symbol map is used to retrieve the numerical ID corressponding to a given symbol.
 std::unordered_map<std::string, uint16_t> symbol_map          {};
 std::vector<uint16_t>                     value_vector        {};
 uint16_t                                  loc                 {0};
 std::uint16_t                             m_count             {};
};

#endif // EMULATED_VM_HPP
