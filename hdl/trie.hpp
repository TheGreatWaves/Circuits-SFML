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
#ifndef DATASTRUCTURE_TRIE
#define DATASTRUCTURE_TRIE

#include <array>

constexpr std::size_t ALPHABET_SIZE {26};

/**
 * A simple trie node.
 */
struct TrieNode 
{
  /**
   * This is only for debugging.
   */
  char letter{};

  /**
   * All possible nodes you can travel 
   * down to from the current node.
   */
  std::array<TrieNode*, ALPHABET_SIZE> children{};

  /**
   * This denotes the the current node is a leaf.
   */
  bool end_of_word{};

  /**
   * METHODS.
   */

  /**
   * Returns a newly created trie node.
   */
  [[ nodiscard ]] constexpr static TrieNode make(const char letter) noexcept
  {
    return {
      .letter = letter,
      .children = {nullptr},
      .end_of_word = false,
    };
  }
};

#endif /* DATASTRUCTURE_TRIE */
