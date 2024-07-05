
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
#ifndef VM_WRITER_HPP
#define VM_WRITER_HPP

#include <string>
#include <sstream>
#include <iostream>

enum class Segment
{
 CONSTANT,
 ARGUMENT,
 LOCAL,
 STATIC,
 THIS,
 THAT,
 POINTER,
 TEMP,
};

enum class Command
{
 ADD,
 SUB,
 NEG,
 EQ,
 GT,
 LT,
 AND,
 OR,
 NOT,
};

class VMWriter
{
public:
 auto write_push(const std::string& segment, const std::string& index) -> void
 {
  buffer << '\t' << "push " << segment << ' ' << index << '\n';
 }

 auto write_pop(const std::string& segment, std::string& index) -> void
 {
  buffer << '\t' << "pop " << segment << ' ' << index << '\n';
 }

 auto write_arithmethic(const std::string& command) -> void
 {
  buffer << '\t' << command << '\n';
 }

 auto write_label(const std::string& label) -> void
 {
  buffer << '\t' << "label " << label << '\n';
 }

 auto write_goto(const std::string& label) -> void
 {
  buffer << '\t' << "goto " << label << '\n';
 }

 auto write_if(const std::string& label) -> void
 {
  buffer << '\t' << "if-goto " << label << '\n';
 }

 auto write_call(const std::string& name, const std::string& n_args) -> void
 {
  buffer << '\t' << "call " << name << ' ' << n_args << '\n';
 }

 auto write_function(const std::string& name, const std::string& n_vars) -> void
 {
  buffer << "function " << name << ' ' << n_vars << '\n';
 }

 auto write_return() -> void
 {
  buffer << "\treturn\n";
 }

 auto out() -> void
 {
  std::cout << buffer.str() << '\n';

 }

 auto build() -> std::string
 {
  return buffer.str();
 }

private:
 std::stringstream buffer {};
};

#endif /* VM_WRITER_HPP */
