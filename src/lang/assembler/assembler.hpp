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
#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <cstdio>
#include <map>
#include <iomanip>

#include "../core/parser_base.hpp"
#include "token_assembler.hpp"

constexpr uint16_t C_INSTRUCTION_OP = 0b111 << 13;
constexpr std::string_view DESTINATION[8] { "null", "M", "D", "MD", "A", "AM", "AD", "AMD", };
constexpr std::string_view JUMP[8] { "null", "JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP" };
inline std::map<std::string_view, std::size_t> COMP {
{"0"  , 0b0101010}, 
{"1"  , 0b0111111}, 
{"-1" , 0b0111010},
{"D"  , 0b0001100}, 
{"A"  , 0b0110000}, 
{"M"  , 0b1110000}, 
{"!D" , 0b0001101},
{"!A" , 0b0110001}, 
{"!M" , 0b1110001}, 
{"-D" , 0b0001111}, 
{"-A" , 0b0110011},
{"-M" , 0b1110011},
{"D+1", 0b0011111}, 
{"A+1", 0b0110111}, 
{"M+1", 0b1110111}, 
{"D-1", 0b0001110},
{"A-1", 0b0110010}, 
{"M-1", 0b1110010}, 
{"D+A", 0b0000010}, 
{"D+M", 0b1000010}, 
{"D-A", 0b0010011},
{"D-M", 0b1010011},
{"A-D", 0b0000111}, 
{"M-D", 0b1000111}, 
{"D&A", 0b0000000}, 
{"D&M", 0b1000000}, 
{"D|A", 0b0010101},
{"D|M", 0b1010101},
};

inline auto get_dest_bits(std::string_view target) -> uint16_t
{
 for (std::uint16_t i{0}; i < 8; i++)
  if (target==DESTINATION[i])
  {
   return i;
  }

 // Error. (Have to check for i > 7)
 return -1;
}

inline auto get_comp_bits(std::string_view target) -> uint16_t
{
 if (auto f = COMP.find(target); f != COMP.end())
 {
  return f->second;
 }
 else
 {
  // Not found.
  return -1;
 }
}

inline auto get_jump_bits(std::string_view target) -> uint16_t
{
 for (std::uint16_t i{0}; i < 8; i++)
  if (target==JUMP[i])
  {
   return i;
  }

 // Error. (Have to check for i > 7)
 return -1;
}

class CodeBuilder
{
public:

 auto add_a_instruction(const std::size_t line, const std::string& varname) -> void
 {
  a_instruction.emplace_back(line, varname);
 }

 auto create_c_instruction(uint8_t comp, uint8_t dest, uint8_t jump) -> uint16_t
 {
  uint16_t instruction = C_INSTRUCTION_OP;
  instruction |= comp << 6;
  instruction |= dest << 3;
  instruction |= jump;
  return instruction;
 }

 auto add_c_instruction(const std::size_t line, uint16_t instruction) -> void
 {
  c_instruction.emplace_back(line, instruction);
 }

 [[nodiscard]] auto to_instructions(const std::unordered_map<std::string, std::size_t>& index_names) const -> const std::array<uint16_t, 32768> 
 {
  std::array<uint16_t, 32768> instructions {0};

  const auto a_size = a_instruction.size();
  const auto c_size = c_instruction.size();
  const auto total_size = a_size + c_size;
  auto a_index{0};
  auto b_index{0};
  auto acc_index {0};

  while (acc_index < total_size)
  {
   const auto& [pos_a, a_name] = (!a_instruction.empty() && a_index < a_size) ? a_instruction.at(a_index) : std::pair<std::size_t, std::string>{-1, ""};
   const auto& [pos_b, b_instr] = (!c_instruction.empty() && b_index < c_size) ? c_instruction.at(b_index) : std::pair<std::size_t, uint16_t>{-1, -1};

   if (pos_a == (acc_index))
   {
    a_index++;
    auto value {0};

    if (isdigit(a_name[0]))
    {
     value = std::stoi(a_name);
    }
    else
    {
     value = index_names.at(a_name);
    }

    // print_number(value);
    instructions[acc_index] = value;
   }
   else if (pos_b == (acc_index))
   {
    b_index++;
    // print_number(b_instr);
    instructions[acc_index] = b_instr;
   }
   else
   {
    std::cerr << "Something went really wrong...\n";
   }

   acc_index++;
  }

  return instructions;
 }

 auto print_number(uint16_t n) const -> void
 {
   for (std::size_t i{0}; i < 16; i++)
   {
    std::cout << ((n >> (15-i)) & 1);
    if (((i+1)%4)==0) std::cout << " ";
   }
  std::cout << '\n';
 }

 auto instruction_count() const -> const uint16_t 
 {
  return a_instruction.size() + c_instruction.size();
 }

private:
 using Index = std::size_t;
 std::vector<std::pair<Index, std::string>> a_instruction{};
 std::vector<std::pair<Index, uint16_t>>    c_instruction{};
};

class Assembler : public BaseParser<AssemblerTokenType>
{
private:
 using TokenType = AssemblerTokenType;

public:
 /**
  * Constructor with file path of .asm file.
  */
 [[nodiscard]] explicit Assembler(const std::string& file_path)
     : BaseParser<AssemblerTokenType>(file_path)
     , next_var_index{16}
 {
  setup_mappings();
 }

 /**
  * Constructor with file path of .asm file.
  */
 [[nodiscard]] explicit Assembler()
     : BaseParser<AssemblerTokenType>()
     , next_var_index{16}
 {
  setup_mappings();
 }

 auto set_source(const std::string& source) noexcept -> void
 {
  this->scanner.set_source(source);
 }

 auto setup_mappings() noexcept -> void
 {
  // Add register names (R0, R1, ...)
  for (std::size_t index {0}; index < 16; index++)
  {
   std::stringstream ss;
   ss << "R" << index;
   this->index_mapping[ss.str()] = index;
   this->index_mapping_inverse[index] = ss.str();
  }

  // Add pointer names.
  this->index_mapping["SP"] = 0;
  this->index_mapping_inverse[0] = "SP";

  this->index_mapping["LCL"] = 1;
  this->index_mapping_inverse[1] = "LCL";

  this->index_mapping["ARG"] = 2;
  this->index_mapping_inverse[2] = "ARG";

  this->index_mapping["THIS"] = 3;
  this->index_mapping_inverse[3] = "THIS";

  this->index_mapping["THAT"] = 4;
  this->index_mapping_inverse[4] = "THAT";
 }

 /**
  * Parse the source code and excute the instructions.
  */
 [[nodiscard]] auto parse() noexcept -> bool
 {
  advance();

  while (!match(AssemblerTokenType::EndOfFile))
  {
   instruction();
  }

  for (auto i {0}; i < 10000; i++)
  {
   if (index_mapping_inverse.contains(i))
   {
    const auto& key = index_mapping_inverse.at(i);
    // std::cout << "Index map: " << key << " " << i << '\n';
    
   }
  }

  std::cout << '\n';

  return !this->has_error;
 }

 auto add_index_mapping(const std::string& varname, std::size_t index)
 {
  // std::cout << "Adding " << varname << " as " << index << '\n';
  this->index_mapping[varname] = index;
  this->index_mapping_inverse[index] = varname;
 }

 auto restabilize() noexcept -> void
 {
     // Reset panic flag, since now we can report a new error for a different part.
     this->panic = false;

     while (this->current.type != TokenType::EndOfFile)
     {
         switch (this->current.type)
         {
             case TokenType::At:
                 return;
             default:; // Do nothing...
         }
         this->advance();
     }
 }


 auto instruction() -> void
 {
  if (match(TokenType::At))
  {
   if (match(TokenType::Number))
   {
    handle_raw_address();
   }
   else if (match(TokenType::Identifier))
   {
    handle_variable();
   }
   else
   {
    report_error("Expected identifier/address after '@'");
   }
   loc++;
  }
  else if (check(TokenType::Number, 
                 TokenType::Identifier, 
                 TokenType::Bang, 
                 TokenType::Minus))
  {
   uint16_t comp = get_compute_expression();
   uint16_t jmp {0};

   const std::string_view destination_reg = this->previous.lexeme;
   uint16_t dest = get_dest_bits(destination_reg);

   if (match(TokenType::Assignment))
   {
    comp = get_compute_expression();
   }
   else if (match(TokenType::Semicolon))
   {
    dest = 0;
    consume(TokenType::Identifier, "Expected jump condition");
    const std::string jump_dest = std::string(this->previous.lexeme);
    jmp = get_jump_bits(jump_dest);
   }

   const auto instr = builder.create_c_instruction(comp, dest, jmp);
   builder.add_c_instruction(loc, instr);
   loc++;
  }
  else if (match(TokenType::LeftParen))
  {
   consume(TokenType::Identifier, "Expected label name, found: " + std::string(this->current.lexeme));
   std::stringstream label_name_ss {};
   label_name_ss << this->previous.lexeme;

   if (match(TokenType::Dot))
   {
    if (!check(TokenType::Identifier, TokenType::Number))
    {
     report_error("Expected identifier, found: " + std::string(this->current.lexeme));
    }
    advance();
    label_name_ss << "." <<  this->previous.lexeme;

    if (match(TokenType::Dollar))
    {
     label_name_ss << "$";
     consume(TokenType::Identifier, "Expected identifier, found: " + std::string(this->current.lexeme));
     label_name_ss << this->previous.lexeme;
     consume(TokenType::Dot, "Expected '.' after return specifier");
     consume(TokenType::Number, "Expected identifier, found: " + std::string(this->current.lexeme));
     label_name_ss << "." << this->previous.lexeme;
    }
   }

   const auto label_name = label_name_ss.str();

   if (index_mapping.contains(label_name))
   {
    // We previously declared it as a variable, so now we have to actually remove it and decrement the index of all variables 
    // whose index is greater than the label's initial index.
    // std::cout << "Label name: " << label_name << " already exists, removing...\n";

    auto index = index_mapping.at(label_name) + 1;
    index_mapping_inverse.erase(index - 1);

    // std::cout << "Purging everything starting from index: " << index << '\n';

    while (index_mapping_inverse.contains(index))
    {
     const auto varname = index_mapping_inverse.at(index);
     index_mapping_inverse.erase(index);
     add_index_mapping(varname, index-1);

     index += 1;
    }

    next_var_index--;
   }

   add_index_mapping(label_name, loc);
   consume(TokenType::RightParen, "Expected enclosing parenthesis ')', found: " + std::string(this->current.lexeme));
  }
  else
  {
   report_error("Invalid token: " + this->current.lexeme);
  }

  if (this->panic)
  {
   restabilize();
  }
 }

 auto get_compute_expression() -> uint16_t
 {
   std::stringstream rhs {};

   if (check(TokenType::Minus, TokenType::Bang))
   {
     rhs << std::string(this->current.lexeme);
     advance();
   }

   if (check(TokenType::Number, TokenType::Identifier))
   {
     rhs << std::string(this->current.lexeme);
     advance();
   }

   if (check(TokenType::Plus, TokenType::Minus, TokenType::And, TokenType::Or))
   {
     rhs << std::string(this->current.lexeme);
     advance();

     if (check(TokenType::Identifier, TokenType::Number))
     {
      rhs << std::string(this->current.lexeme);
      advance();
     }
   }

   return get_comp_bits(rhs.str());
 }

 
 auto handle_raw_address() -> void 
 {
  const auto address_str = std::string(this->previous.lexeme);
  builder.add_a_instruction(loc, address_str);
 }

 auto resolve_variable(const std::string& varname) -> std::size_t
 {
  if (!index_mapping.contains(varname))
  {
   this->add_index_mapping(varname, next_var_index++);
  }

  return index_mapping[varname];
 }

 auto handle_variable() -> void
 {
   std::stringstream varname_ss {};
   varname_ss << this->previous.lexeme;

   if (match(TokenType::Dot))
   {
    if (!check(TokenType::Identifier, TokenType::Number))
    {
     report_error("Expected identifier, found: " + std::string(this->current.lexeme));
    }
    advance();
    varname_ss << "." <<  this->previous.lexeme;

    if (match(TokenType::Dollar))
    {
     varname_ss << "$";
     consume(TokenType::Identifier, "Expected identifier, found: " + std::string(this->current.lexeme));
     varname_ss << this->previous.lexeme;
     consume(TokenType::Dot, "Expected '.' after return specifier");
     consume(TokenType::Number, "Expected identifier, found: " + std::string(this->current.lexeme));
     varname_ss << "." << this->previous.lexeme;
    }
   }

  const auto varname = varname_ss.str();

  resolve_variable(varname);
  builder.add_a_instruction(loc, varname);
 }

 [[nodiscard]] auto to_instructions() const -> const std::array<uint16_t, 32768>
 {
  return this->builder.to_instructions(index_mapping);
 }

 auto print_code() const -> void
 {
  const auto instructions = to_instructions();

  for (auto i {0}; i < this->builder.instruction_count(); i++)
  {
   auto instruction = instructions[i];
   std::cout << std::setw(5) << std::right << instruction << " | ";
   builder.print_number(instruction);
  }
 }

 auto code() const -> const CodeBuilder& 
 {
  return builder;
 }

 private:


 /**
  * Variable name and address mapping.
  */ 
 std::unordered_map<std::string, std::size_t> index_mapping         {};
 std::unordered_map<std::size_t, std::string> index_mapping_inverse {};
 std::size_t                                  next_var_index        {16};
 uint16_t                                     loc                   {0};

 /**
  * Binary code generator.
  */
 CodeBuilder builder{};

};

#endif
