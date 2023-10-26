#include <iostream>

#include "trie.hpp"

int main()
{
  TrieNode node = TrieNode::make('d');
  std::cout << "Trie node: " << node.letter << '\n';
  return 0;
}