#include <iostream>

#include "../core/token_base.hpp"
#include "token_hdl.hpp"

auto main() -> int
{
 for (const auto& token : HDLTokenType::all_tokens) 
 {
  if (token.is_keyword()) std::cout << "[ KEYWORD ] " << token.name() << " '" << token.get_symbol() << "'\n";
  else if (token.is_symbol()) std::cout << "[ SYMBOL ] " << token.name() << " '" << token.get_symbol() << "'\n";
  else std::cout << "[ TOKEN ] " << token.name() << '\n';
 }

 std::cout << "==================================\n";

 for (const auto& token : HDLTokenType::keyword_tokens) 
 {
  std::cout << "[ KEYWORD TOKEN ] " << token.name() << " " << token.get_symbol() << '\n';
 }
 
}