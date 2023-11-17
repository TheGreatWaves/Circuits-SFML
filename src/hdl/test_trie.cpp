#include <iostream>

#include "trie.hpp"

auto main() -> int {
 Trie trie{};
 auto hello = "what";
 trie.insert(hello);
 trie.insert("what should not break");
 if (trie.search("what"))
 {
  // do nothing...
  std::cout << "found!\n";
 }

 if (trie.search("what should not break"))
 {
  // do nothing...
  std::cout << "found!\n";
 }
}