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
#ifndef HDL_META
#define HDL_META

#include <string>
#include <string_view>
#include <vector>

#include "trie.hpp"

namespace hdl
{

/**
 * The Meta struct holds relevant data needed by the .hdl parser.
 */
struct Meta
{
    struct PinEntry
    {
        std::string pin_name;
        std::size_t pin_number;
    };

    const std::string     name;
    std::vector<PinEntry> input_pins;
    std::vector<PinEntry> output_pins;
    Trie                  trie;
};

/**
 * Built in meta for NAND.
 */
inline static const Meta NAND_GATE = Meta{
    .name        = "Nand",
    .input_pins  = {{"a", 0}, {"b", 1}},
    .output_pins = {{"out", 100}},
    .trie        = Trie{"Nand"},
};

[[nodiscard]] inline auto get_meta(std::string_view component_name) -> const Meta
{
    if (component_name == "Nand")
    {
        return NAND_GATE;
    }
    else
    {

    }
}

[[nodiscard]] inline auto generate_meta_from_path(std::string_view component_name) -> const Meta
{

}



} /* namespace hdl */

#endif /* HDL_META */
