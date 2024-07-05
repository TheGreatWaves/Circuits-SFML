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

#include "token_jack.hpp"
#include "../core/parser_base.hpp"

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

 auto compile_term() -> void
 {
  const auto scope = write_scope_newline("term");

  // Prefix
  if (check(TokenType::Minus, TokenType::Tilda))
  {
   advance();
   write_previous();
   compile_term();
  }
  // Number
  else if (match(TokenType::Number))
  {
   write_previous();
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
   write_previous();

   if (match(TokenType::LSqaure))
   {
    write_previous();
    compile_expression();
    
    consume(TokenType::RSquare, "Expected ']', array subscript not terminated");
    write_previous();
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

    // Subroutine name
    read_identifier();
    write_previous();

    consume(TokenType::LParen, "Expected '(' to mark the beginning of parameter list");
    write_previous();

    compile_expression_list();

    consume(TokenType::RParen, "Expected ')' to mark the end of parameter list");
    write_previous();
   }
  }
  else if (check(TokenType::True, TokenType::False, TokenType::Null, TokenType::This))
  {
   // Keyword constant
   advance();
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

 auto is_op() -> bool 
 {
  return check(
               TokenType::Plus, 
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

 auto compile_expression() -> void
 {
  const auto scope = write_scope_newline("expression");

  compile_term();

  while (is_op())
  {
   advance();
   write_previous();
   compile_term();
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

  read_identifier();

  do {
   write_previous();
   read_identifier();
   write_previous();
  } while (match(TokenType::Comma));

  consume(TokenType::Semicolon, "Expected ';' at the end of variable declaration");
  write_token(previous);
 }

 auto compile_subroutine_dec() -> void
 {
  const auto scope =write_scope_newline("subroutineDec");
  write_previous();

  // Return type name
  read_identifier();
  write_previous();

  // Method name
  read_identifier();
  write_previous();

  consume(TokenType::LParen, "Expected '(' to begin argument list after method name");
  write_previous();

  compile_parameter_list();

  consume(TokenType::RParen, "Expected ')' at the end of parameter list");
  write_previous();

  compile_subroutine_body();
 }

 auto compile_parameter_list() -> void
 {
  const auto scope = write_scope_newline("parameterList");

  if (check(TokenType::RParen)) return;

  while (!match(TokenType::EndOfFile))
  {
   // Type
   read_identifier();
   write_previous();

   // Name
   read_identifier();
   write_previous();

   if (!match(TokenType::Comma))
    break;
   write_previous();
  } 
 }

 auto compile_subroutine_body() -> void
 {
  const auto scope = write_scope_newline("subroutineBody");

  consume(TokenType::LBrace, "Expected '{' at the start of subroutine body");
  write_previous();

  while (!check(TokenType::RBrace, TokenType::EndOfFile))
  {
   while (match(TokenType::Var))
    compile_var_dec();

   compile_statements();
  }

  consume(TokenType::RBrace, "Expected '}' at the end of subroutine body");
  write_previous();

  
 }

 auto compile_var_dec() -> void
 {
  const auto scope = write_scope_newline("varDec");

  // Var
  write_previous();

  // Type
  read_identifier();

  // Variable names
  do {
   write_previous();
   read_identifier();
   write_previous();
  } while (match(TokenType::Comma));

  consume(TokenType::Semicolon, "Expected ';' at the end of variable declaration");
  write_token(previous);
 }

 auto compile_let() -> void
 {
  const auto scope = write_scope_newline("letStatement");
  write_previous();

  // Varname
  read_identifier();
  write_previous();

  if (match(TokenType::LSqaure))
  {
   write_previous();
   compile_expression();
   consume(TokenType::RSquare, "Expected ']' after array subscript");
   write_previous();
  }

  consume(TokenType::Assignment, "Expected '=' in let statement");
  write_previous();

  compile_expression();

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

  consume(TokenType::RParen, "Expected ')' after if condition");
  write_previous();

  
  consume(TokenType::LBrace, "Expected '{' before if body");
  write_previous();

  compile_statements();

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
 }

 auto compile_expression_list() -> void
 {
  const auto scope = write_scope_newline("expressionList");

  // Argument list can be empty.
  if (check(TokenType::RParen)) return;

  while (true) 
  {
   compile_expression();
   if (!match(TokenType::Comma)) break;
   write_previous();
  }
 }

 auto compile_do() -> void
 {
  const auto scope = write_scope_newline("doStatement");
  write_previous();

  // Subroutine name
  read_identifier();
  write_previous();

  if (match(TokenType::Dot))
  {
   write_previous();
   read_identifier();
   write_previous();
  }

  consume(TokenType::LParen, "Expected '(' after subroutine name");
  write_previous();

  compile_expression_list();

  consume(TokenType::RParen, "Expected ')' after expression list");
  write_previous();

  consume(TokenType::Semicolon, "Expected ';' at the end of do statement");
  write_previous();
 }

 auto compile_return() -> void
 {
  const auto scope = write_scope_newline("returnStatement");
  write_previous();

  if (!check(TokenType::Semicolon))
   compile_expression();

  consume(TokenType::Semicolon, "Expected ';' at the end of return statement");
  write_previous();
 }  

 auto compile_class() -> void
 {
  auto scope = write_scope_newline("class");
  write_previous();

  read_identifier();
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

  consume(TokenType::LParen, "Expected '(' after while");
  write_previous();

  compile_expression();

  consume(TokenType::RParen, "Expected ')' after while condition");
  write_previous();

  consume(TokenType::LBrace, "Expected '{' at the beginning of while body");
  write_previous();

  compile_statements();

  consume(TokenType::RBrace, "Expected '}', while body not terminated");
  write_previous();
 }

 auto write_head(std::string_view element) -> void
 {
  tabs();
  buffer << "<" << element << ">";
 }

 auto write_newline() -> void
 {
  buffer << '\n';
 }

 auto tabs() -> void
 {
  buffer << std::string(depth, ' ') << std::string(depth, ' ');
 }

 auto write_tail(std::string_view element) -> void
 {
  tabs();
  buffer << "<" << "/" << element << ">\n";
 }

 auto output() -> void
 {
  buffer << '\n';
  std::cout << buffer.str();
 }

 auto write_scope_newline(std::string_view element) -> ScopeCallback
 {
  write_head(element);
  write_newline();
  this->depth += 1;
  return ScopeCallback([=, this] {
   this->depth -= 1;
   write_tail(element);
  });
 }

 auto write_single(std::string_view element, std::string_view item) -> void
 {
  tabs();
  buffer << '<' << element << "> " << item << " </" << element << ">\n";
 }

 private:
  std::uint16_t     depth {0};
  std::stringstream buffer {};
};

#endif /* JACK_HPP */
