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
#ifndef JACK_HPP
#define JACK_HPP

#include <functional>
#include <optional>
#include <unordered_map>

#include "token_jack.hpp"
#include "../core/parser_base.hpp"
#include "vm_writer.hpp"

enum class SymbolKind
{
 THIS = 0,
 STATIC,
 ARGUMENT,
 LOCAL,
};

inline auto symbol_kind_string(SymbolKind kind) -> std::string 
{
 switch (kind) 
 {
  case SymbolKind::THIS:     return "this";
  case SymbolKind::STATIC:   return "static";
  case SymbolKind::ARGUMENT: return "argument";
  case SymbolKind::LOCAL:    return "local";
  break; default:            return "";
 }
}


struct TableEntry
{
 std::string   type;
 SymbolKind    kind;
 std::uint16_t index;
};

using SymbolTable = std::unordered_map<std::string, TableEntry>;

struct CompilerContext
{
 std::string class_name;
 SymbolTable class_table;
 SymbolTable method_table;
 uint16_t kind_count[4] {0};

 auto count(SymbolKind kind) -> uint16_t
 {
  return kind_count[static_cast<std::size_t>(kind)]++;
 }

 auto set_name(const std::string& name) -> void
 {
  class_name = name;
 }

 auto add_class_variable(const std::string& name, const std::string& type, SymbolKind kind) -> void
 {
  class_table[name] = TableEntry{.type=type, .kind=kind, .index=count(kind)};
 }

 auto add_method_variable(const std::string& name, const std::string& type, SymbolKind kind) -> void
 {
  method_table[name] = TableEntry{.type=type, .kind=kind, .index=count(kind)};
 }

 auto clear_method_table() -> void
 {
  method_table.clear();

  // Reset counts for local and argument
  kind_count[static_cast<std::size_t>(SymbolKind::ARGUMENT)] = 0;
  kind_count[static_cast<std::size_t>(SymbolKind::LOCAL)] = 0;
 }

 auto new_method() -> void
 {
  clear_method_table();
  method_table["this"] = TableEntry{.type=class_name, .kind=SymbolKind::ARGUMENT, .index=count(SymbolKind::ARGUMENT)};
 }

 auto get_entry(const std::string& identifier) -> TableEntry*
 {
  if (method_table.contains(identifier)) return &method_table[identifier];
  else if (class_table.contains(identifier)) return &class_table[identifier];
  return nullptr;
 }

 auto kind_of(const std::string& identifier) -> std::optional<SymbolKind>
 {
  if (const auto entry = get_entry(identifier); entry != nullptr)
   return entry->kind;

  // Compile error.
  return {};
 }

 auto type_of(const std::string& identifier) -> std::optional<std::string>
 {
  if (const auto entry = get_entry(identifier); entry != nullptr)
   return entry->type;

  // Compile error.
  return {};
 }

 auto index_of(const std::string& identifier) -> std::optional<uint16_t>
 {
  if (const auto entry = get_entry(identifier); entry != nullptr)
   return entry->index;

  // Compile error.
  return {};
 }
};

struct ScopeCallback
{
  explicit ScopeCallback(std::function<void()> f)
  : callback(f)
  {
  }

  ~ScopeCallback()
  {
   callback();
  }

  std::function<void()> callback;
};

// Jack Parser
class JackParser : public BaseParser<JackTokenType> 
{
private:
  using TokenType = JackTokenType;

public:
 explicit JackParser(const std::string& file_path) 
 : BaseParser<JackTokenType>(file_path)
 {
 }

 auto read_identifier() -> std::string
 {
  if (!current.type.is_keyword() && current.type != TokenType::Identifier) 
  {
   report_error("Expected identifier, found: " + current.lexeme);
  }
  advance();

  return std::string(previous.lexeme);
 }

 [[nodiscard]] auto parse() noexcept -> bool
 {
  advance();

  while (!match(TokenType::EndOfFile))
  {
   if (match(TokenType::Class))
    compile_class();

   if (this->panic) advance();
  }

  return !this->has_error;
 }

 auto handle_array_term(const std::string& name) -> void
 {
    const auto entry = m_context.get_entry(name);

    if (entry == nullptr)
    {
     report_error("'" + previous.lexeme + "' undefined");
     return;
    }

    const auto index = std::to_string(entry->index);
    const auto segment = symbol_kind_string(entry->kind);

    m_writer.write_push(segment, index);

    write_previous();
    compile_expression();

    m_writer.write_arithmethic("add");

    m_writer.write_pop("pointer", "1");
    m_writer.write_push("that", "0");
    
    consume(TokenType::RSquare, "Expected ']', array subscript not terminated");
    write_previous();
 }

 auto compile_term() -> void
 {
  const auto scope = write_scope_newline("term");

  // Prefix
  if (check(TokenType::Minus, TokenType::Tilda))
  {
   const auto prefix_type = current.type;
   advance();
   write_previous();
   compile_term();

   if (prefix_type == TokenType::Minus)
    m_writer.write_arithmethic("neg");
   else
    m_writer.write_arithmethic("not");
  }
  // Number
  else if (match(TokenType::Number))
  {
   write_previous();
   m_writer.write_push("constant", previous.lexeme);
  }
  // String
  else if (match(TokenType::String))
  {
   write_previous();
  }
  // Variable/Array/Subroutine Call
  else if (match(TokenType::Identifier))
  {
   // Variable name
   const auto name = previous.lexeme;
   write_previous();

   if (match(TokenType::LSquare))
   {
    handle_array_term(name);
   }
   else if (match(TokenType::LParen))
   {
    write_previous();
    compile_expression_list();
    
    consume(TokenType::RParen, "Expected ')' at the end of subroutine parameters");
    write_previous();
   }
   else if (match(TokenType::Dot))
   {
    write_previous();

    auto count = 0;

    if (m_context.method_table.contains(name))
    {
     count = 1;

     const auto entry = m_context.get_entry(name);
     const auto index = std::to_string(entry->index);
     const auto segment = symbol_kind_string(entry->kind);

     m_writer.write_push(segment, index);
    }

    // Subroutine name
    const auto subroutine_full_name = name + "." + read_identifier();
    write_previous();

    consume(TokenType::LParen, "Expected '(' after subroutine name");
    write_previous();

    const auto function_arg_count = std::to_string(count + compile_expression_list());

    consume(TokenType::RParen, "Expected ')' after expression list");
    write_previous();

    m_writer.write_call(subroutine_full_name, function_arg_count);
   }
   else
   {
    const auto entry = m_context.get_entry(previous.lexeme);

    if (entry == nullptr)
    {
     report_error("'" + previous.lexeme + "' undefined");
     return;
    }

    const auto index = std::to_string(entry->index);
    const auto segment = symbol_kind_string(entry->kind);

    m_writer.write_push(segment, index);
   }
  }
  else if (is_keyword_constant())
  {
   // Keyword constant
   advance();
   handle_keyword_constant(previous.type);
   write_previous();
  }
  else if (match(TokenType::LParen))
  {
   write_previous();

   compile_expression();

   consume(TokenType::RParen, "Expected ')', expresssion not terminated");
   write_previous();
  }
 }

 auto is_keyword_constant() -> bool
 {
  return check(TokenType::True, 
               TokenType::False, 
               TokenType::Null, 
               TokenType::This);
 }

 auto handle_keyword_constant(TokenType type) -> void
 {
  switch (type)
  {
   break; case TokenType::True: 
   {
    m_writer.write_push("constant", "1");
    m_writer.write_arithmethic("neg");
   }
   break; case TokenType::False: m_writer.write_push("constant", "0");
   break; case TokenType::Null: m_writer.write_push("constant", "0");
   break; case TokenType::This: m_writer.write_push("pointer", "0");
   break; default: {report_error("Unhanlded keyword constant case: " + std::string(current.type.name()));}
  }
 }

 auto is_op() -> bool 
 {
  return check(TokenType::Plus, 
               TokenType::Minus, 
               TokenType::Asterisk, 
               TokenType::Slash, 
               TokenType::Ampersand, 
               TokenType::Bar,
               TokenType::LessThan,
               TokenType::GreaterThan,
               TokenType::Assignment
   );
 }

 auto handle_op(TokenType type) -> void
 {
  switch (type)
  {
   break; case TokenType::Plus: m_writer.write_arithmethic("add");
   break; case TokenType::Minus: m_writer.write_arithmethic("sub");
   break; case TokenType::Ampersand: m_writer.write_arithmethic("and");
   break; case TokenType::Bar: m_writer.write_arithmethic("or");
   break; case TokenType::LessThan: m_writer.write_arithmethic("lt");
   break; case TokenType::GreaterThan: m_writer.write_arithmethic("gt");
   break; case TokenType::Assignment: m_writer.write_arithmethic("eq");
   break; case TokenType::Asterisk: m_writer.write_call("Math.multiply", "2");
   break; case TokenType::Slash: m_writer.write_call("Math.divide", "2");
   break; default: {report_error("Unhanlded OP case: " + std::string(current.type.name()));}
  }
 }

 auto compile_expression() -> void
 {
  const auto scope = write_scope_newline("expression");

  compile_term();

  while (is_op())
  {
   const auto op_type = current.type;
   advance();
   write_previous();
   compile_term();
   handle_op(op_type);
  }
 }

 auto compile_statements() -> void
 {
  const auto scope = write_scope_newline("statements");

  while (true)
  {
   if (match(TokenType::Let))
   {
    compile_let();
   }
   else if (match(TokenType::If))
   {
    compile_if();
   }
   else if (match(TokenType::While))
   {
    compile_while();
   }
   else if (match(TokenType::Do))
   {
    compile_do();
   }
   else if (match(TokenType::Return))
   {
    compile_return();
   } 
   else
   {
    break;
   }
  }
 }

 auto write_previous() -> void
 {
  write_token(previous);
 }

 auto write_token(const Token<JackTokenType>& token) -> void
 {
  if (token.type.is_keyword())
  {
   write_single("keyword", token.lexeme);
  }
  else if (token.type.is_symbol())
  {
   if (token.type == TokenType::Ampersand)
    write_single("symbol", "&amp;");
   else if (token.type == TokenType::LessThan)
    write_single("symbol", "&lt;");
   else if (token.type == TokenType::GreaterThan)
    write_single("symbol", "&gt;");
   else
    write_single("symbol", token.lexeme);
  }
  else if (token.type == TokenType::Number)
  {
   write_single("integerConstant", token.lexeme);
  }
  else if (token.type == TokenType::Identifier)
  {
   write_single("identifier", token.lexeme);
  }
  else if (token.type == TokenType::String)
  {
   write_single("stringConstant", token.lexeme);
  }
 }

 auto compile_class_var_dec() -> void
 {
  const auto scope = write_scope_newline("classVarDec");
  write_token(previous);

  const auto variable_kind = previous.type == TokenType::Static
                           ? SymbolKind::STATIC
                           : SymbolKind::THIS;
  const auto variable_type = read_identifier();

  do {
   write_previous();
   const auto variable_name = read_identifier();

   m_context.add_class_variable(variable_name, variable_type, variable_kind);

   write_previous();
  } while (match(TokenType::Comma));

  consume(TokenType::Semicolon, "Expected ';' at the end of variable declaration");
  write_token(previous);
 }

 auto compile_subroutine_dec() -> void
 {
  m_context.clear_method_table();

  const auto subroutine_type = previous.type;
  if (previous.type == TokenType::Method)
   m_context.new_method();

  const auto scope  = write_scope_newline("subroutineDec");
  write_previous();

  // Return type name
  const auto return_type = read_identifier();
  write_previous();

  // Method name
  const auto method_name = read_identifier();
  write_previous();

  consume(TokenType::LParen, "Expected '(' to begin argument list after method name");
  write_previous();

  compile_parameter_list();

  consume(TokenType::RParen, "Expected ')' at the end of parameter list");
  write_previous();

  compile_subroutine_body(subroutine_type, method_name);

  if (return_type == "void") 
  {
   m_writer.write_push("constant", "0");
   m_writer.write_return();
  }
 }

 auto compile_parameter_list() -> std::size_t
 {
  const auto scope = write_scope_newline("parameterList");

  if (check(TokenType::RParen)) return 0;

  auto count = 0;

  while (!match(TokenType::EndOfFile))
  {
   count++;

   // Type
   const auto parameter_type = read_identifier();
   write_previous();

   // Name
   const auto parameter_name = read_identifier();
   write_previous();

   m_context.add_method_variable(parameter_name, parameter_type, SymbolKind::ARGUMENT);

   if (!match(TokenType::Comma))
    break;
   write_previous();
  } 

  return count;
 }

 auto compile_subroutine_body(TokenType subroutine_type, const std::string& method_name) -> void
 {
  const auto scope = write_scope_newline("subroutineBody");

  consume(TokenType::LBrace, "Expected '{' at the start of subroutine body");
  write_previous();

  // TODO: Move this in later, the problem with that is that
  //       the count of variable becomes annoying to deal with.

  auto local_var_count {0};

  while (match(TokenType::Var))
  {
   local_var_count += compile_var_dec();
  }

  m_writer.write_function(m_context.class_name+"."+method_name, std::to_string(local_var_count));

  if (subroutine_type == TokenType::Constructor)
  {
   m_writer.write_push("constant", std::to_string(m_context.count(SymbolKind::THIS)));
   m_writer.write_call("Memory.alloc", "1");
   m_writer.write_pop("pointer", "0");
  }
  else if (subroutine_type == TokenType::Method)
  {
   m_writer.write_push("argument", "0");
   m_writer.write_pop("pointer", "0");
  }

  while (!check(TokenType::RBrace, TokenType::EndOfFile))
  {
   compile_statements();
  }

  consume(TokenType::RBrace, "Expected '}' at the end of subroutine body");
  write_previous();
 }

 auto compile_var_dec() -> int
 {
  const auto scope = write_scope_newline("varDec");
  auto count = 0;

  // Var
  write_previous();

  // Type
  const auto variable_type = read_identifier();

  // Variable names
  do {
   count++;
   write_previous();

   const auto variable_name = read_identifier();

   m_context.add_method_variable(variable_name, variable_type, SymbolKind::LOCAL);

   write_previous();
  } while (match(TokenType::Comma));

  consume(TokenType::Semicolon, "Expected ';' at the end of variable declaration");
  write_token(previous);

  return count;
 }

 auto compile_let() -> void
 {
  const auto scope = write_scope_newline("letStatement");
  write_previous();

  // Varname
  const auto variable_name = read_identifier();
  write_previous();

  const auto entry = m_context.get_entry(variable_name);

  if (entry == nullptr)
  {
   report_error("'" + variable_name + "' undefined");
   return;
  }

  const auto index = std::to_string(entry->index);
  const auto segment = symbol_kind_string(entry->kind);
  const auto is_arr_manip = current.type == TokenType::LSquare;

  if (is_arr_manip)
  {
   advance();
   write_previous();
   m_writer.write_push(segment, index);
   compile_expression();
   consume(TokenType::RSquare, "Expected ']' after array subscript");
   write_previous();

   m_writer.write_arithmethic("add");
  }

  consume(TokenType::Assignment, "Expected '=' in let statement");
  write_previous();

  compile_expression();

  if (is_arr_manip) 
  {
   m_writer.write_pop("temp", "0");
   m_writer.write_pop("pointer", "1");
   m_writer.write_push("temp", "0");
   m_writer.write_pop("that", "0");
  }
  else
  {
   m_writer.write_pop(segment, index);
  }

  consume(TokenType::Semicolon, "Expected ';' at the end of let statement, found: " + current.lexeme);
  write_previous();
 }

 auto compile_if() -> void
 {
  const auto scope = write_scope_newline("ifStatement");
  write_previous();

  consume(TokenType::LParen, "Expected '(' before if condition");
  write_previous();

  compile_expression();

  m_writer.write_arithmethic("not");

  const auto end_of_if = create_label("EOI");
  const auto end_of_else = create_label("EOE");
  m_writer.write_if(end_of_if);

  consume(TokenType::RParen, "Expected ')' after if condition");
  write_previous();

  
  consume(TokenType::LBrace, "Expected '{' before if body");
  write_previous();

  compile_statements();

  m_writer.write_goto(end_of_else);
  m_writer.write_label(end_of_if);

  consume(TokenType::RBrace, "Expected '}' after if body");
  write_previous();

  if (match(TokenType::Else))
  {
   write_previous();
   consume(TokenType::LBrace, "Expected '{' before else body");
   write_previous();

   compile_statements();

   consume(TokenType::RBrace, "Expected '}' after else body");
   write_previous();
  }

  m_writer.write_label(end_of_else);
 }

 auto compile_expression_list() -> std::size_t
 {
  const auto scope = write_scope_newline("expressionList");

  // Argument list can be empty.
  if (check(TokenType::RParen)) return 0;

  auto count {0};

  while (true) 
  {
   compile_expression();
   count++;
   if (!match(TokenType::Comma)) break;
   write_previous();
  }

  return count;
 }

 auto compile_do() -> void
 {
  const auto scope = write_scope_newline("doStatement");
  write_previous();

  compile_expression();

  consume(TokenType::Semicolon, "Expected ';' at the end of do statement");
  write_previous();

  m_writer.write_pop("temp", "0");
 }

 auto compile_return() -> void
 {
  const auto scope = write_scope_newline("returnStatement");
  write_previous();

  if (check(TokenType::Semicolon))
   m_writer.write_push("constant", "0"); // Push dummy value
  else
   compile_expression();                 // Push expression

  m_writer.write_return();

  consume(TokenType::Semicolon, "Expected ';' at the end of return statement");
  write_previous();
 }  

 auto compile_class() -> void
 {
  auto scope = write_scope_newline("class");
  write_previous();

  // Class name
  const auto class_name = read_identifier();
  m_context.set_name(class_name);
  write_previous();

  consume(TokenType::LBrace, "Expected '{' before class body");
  write_previous();

  while (!check(TokenType::RBrace, TokenType::EndOfFile))
  {
   if (check(TokenType::Static, TokenType::Field))
   {
    advance();
    compile_class_var_dec();
   }
   else if (check(TokenType::Constructor, TokenType::Function, TokenType::Method))
   {
    advance();
    compile_subroutine_dec();
   }
  }

  consume(TokenType::RBrace, "Expected '}' after class body");
  write_previous();
 }

 auto compile_while() -> void
 {
  const auto scope = write_scope_newline("whileStatement");
  write_previous();

  // while body label
  const auto body_label = create_label("wbl");

  // while exit label
  const auto exit_label = create_label("wel");

  m_writer.write_label(body_label);

  consume(TokenType::LParen, "Expected '(' after while");
  write_previous();

  compile_expression();

  m_writer.write_arithmethic("not");


  // If condition fails, jump to exit
  m_writer.write_if(exit_label);

  consume(TokenType::RParen, "Expected ')' after while condition");
  write_previous();

  consume(TokenType::LBrace, "Expected '{' at the beginning of while body");
  write_previous();

  compile_statements();

  // Loop
  m_writer.write_goto(body_label);

  // Exit label placement
  m_writer.write_label(exit_label);

  consume(TokenType::RBrace, "Expected '}', while body not terminated");
  write_previous();
 }

 auto write_head(std::string_view element) -> void
 {
  tabs();
  m_buffer << "<" << element << ">";
 }

 auto write_newline() -> void
 {
  m_buffer << '\n';
 }

 auto tabs() -> void
 {
  m_buffer << std::string(m_depth, ' ') << std::string(m_depth, ' ');
 }

 auto write_tail(std::string_view element) -> void
 {
  tabs();
  m_buffer << "<" << "/" << element << ">\n";
 }

 auto build() -> std::string
 {
  return m_writer.build();
 }

 auto output() -> void
 {
  // // m_buffer << '\n';
  // // std::cout << m_buffer.str();
  // for (const auto& [k, v] : m_context.class_table)
  // {
  //  std::cout << "key: " << k << " " << v.type << ' ' << static_cast<uint16_t>(v.kind) << ' ' << v.index << '\n';
  // }
  // std::cout << '\n';
  // for (const auto& [k, v] : m_context.method_table)
  // {
  //  std::cout << "key: " << k << " " << v.type << ' ' << static_cast<uint16_t>(v.kind) << ' ' << v.index << '\n';
  // }
  m_writer.out();
 }

 auto write_scope_newline(std::string_view element) -> ScopeCallback
 {
  write_head(element);
  write_newline();
  this->m_depth += 1;
  return ScopeCallback([=, this] {
   this->m_depth -= 1;
   write_tail(element);
  });
 }

 auto write_single(std::string_view element, std::string_view item) -> void
 {
  tabs();
  m_buffer << '<' << element << "> " << item << " </" << element << ">\n";
 }

 auto create_label(const std::string& label_name) -> std::string 
 {
  if (!m_label_count.try_emplace(label_name, 0).second)
   m_label_count[label_name]++;
  return label_name + '_' + std::to_string(m_label_count[label_name]);
 }

 private:
  std::uint16_t     m_depth   {0};
  std::stringstream m_buffer  {};
  CompilerContext   m_context {};
  std::unordered_map<std::string, uint16_t> m_label_count;
 public:
  VMWriter          m_writer  {};

};

#endif /* JACK_HPP */
