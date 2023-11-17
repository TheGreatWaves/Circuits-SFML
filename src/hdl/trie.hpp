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
#include <string>
#include <string_view>
#include <vector>

/**
 * Case sentitive!
 * Lowercase and uppercase letters.
 */
constexpr std::size_t ASCII_COUNT{256};

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
    std::array<TrieNode*, ASCII_COUNT> children{};

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
    [[nodiscard]] static inline TrieNode* make(const char letter) noexcept
    {
        auto trie = new TrieNode{
            .letter = letter,
            .children =
                {
                    nullptr,
                },
            .end_of_word = false,
        };

        for (std::size_t i = 0; i < ASCII_COUNT; i++)
        {
          trie->children[i] = nullptr;
        }

        return trie;
    }
};

/**
 * The Trie class holds one root TrieNode.
 */
class Trie
{
  public:
    /**
     * Create a new trie.
     */
    Trie() noexcept
        : root{TrieNode::make('\0')}
    {
    }

    /**
     * Create a new trie with one value.
     */
    Trie(const std::string& word) noexcept
        : root{TrieNode::make('\0')}
    {
        insert(word);
    }

    /**
     * Create a new trie with multiple value.
     */
    Trie(const std::vector<std::string> words) noexcept
        : root{TrieNode::make('\0')}
    {
        for (const auto& word : words)
        {
            insert(word);
        }
    }

    /**
     * Delete the root node and all of it's children.
     */
    ~Trie() noexcept
    {
        if (this->root != nullptr)
         clean_up(this->root);
    }

    /**
     * Clean up tree.
     */
    void clean_up(TrieNode* node) noexcept
    {
        if (node == nullptr)
            return;
        for (std::size_t i = 0; i < ASCII_COUNT; i++)
        {
            clean_up(node->children[i]);
        }
        delete node;
    }

    /**
     * Inserts the word into the trie.
     */
    void insert(const std::string& word) noexcept
    {
        TrieNode* current = this->root;

        for (auto i = 0; word[i] != '\0'; i++)
        {
            const char letter = word[i];
            auto       index  = static_cast<unsigned int>(letter);

            /**
             * If the current character's branch does not exist, create it.
             */
            if (current->children[index] == nullptr)
            {
                current->children[index] = TrieNode::make(letter);
            }
            current = current->children[index];
        }

        /**
         * By this point, we would have reached the end of the word.
         * Now we can just must this position as a leaf node.
         */
        current->end_of_word = true;
    }

    /**
     * Search for the given word. Returns true if found.
     */
    [[nodiscard]] bool search(const std::string& word) noexcept
    {
        TrieNode* current = this->root;

        for (auto i = 0; word[i] != '\0'; i++)
        {
            const char letter = word[i];
            auto       index  = static_cast<unsigned int>(letter);

            /**
             * If the current character's branch does not exist, not found.
             */
            if (current->children[index] == nullptr)
            {
                return false;
            }
            current = current->children[index];
        }

        return current->end_of_word;
    }

    /**
     * Check the word against an expected entry.
     */
    [[nodiscard]] bool match(const std::string& expected_word, const std::string& word) const noexcept
    {
        if (expected_word.length() != word.length())
            return false;

        TrieNode* current = this->root;

        for (auto i = 0; word[i] != '\0'; i++)
        {
            const char expected_letter = expected_word[i];
            const char letter          = word[i];
            if (expected_letter != letter)
                return false;

            auto index = static_cast<unsigned int>(letter);

            /**
             * If the current character's branch does not exist, not found.
             */
            if (current->children[index] == nullptr)
            {
                return false;
            }
            current = current->children[index];
        }

        return current->end_of_word;
    }

  private:
    TrieNode* root;
};

#endif /* DATASTRUCTURE_TRIE */
