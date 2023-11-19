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
#ifndef BOARD
#define BOARD

#include "gate.hpp"
#include "utils.hpp"
#include <memory>
#include <map>

// We have a single static instance of the board (this will have the lifetime of the program)
class Board
{
public:
  Board()
  {
    auto nandc = std::make_unique<Gate>(2, 1, GateType::NAND, "nand", true);
    auto nandp = nandc.get();
    components["nand"] = std::move(nandc);
    auto nand = components["nand"].get();
    nand->get_pin(0)->parent = nandp;
    nand->get_pin(1)->parent = nandp;
    nand->serialize();

    singleton = this;
  }

  ~Board()
  {
    singleton = nullptr;
  }

  static Board* instance()
  {
    return singleton;
  }

  void create_new(std::string_view name)
  {
    auto gate = std::make_unique<Gate>();
    gate->set_name(name);
    components[make_lower(name)] = std::move(gate);
  }

  void reset_context()
  {
    current = std::pair{"", nullptr};
  }

  void set_context(std::string_view name)
  {
    const auto case_insensitive_name = make_lower(name);
    bool f = found(case_insensitive_name);
    std::string entry{ case_insensitive_name };
    current = f ? std::pair(case_insensitive_name, components[entry].get()) : std::pair{"", nullptr};
  }

  bool found(std::string_view name)
  {
    std::string entry{ name };
    return components.find(entry) != components.end();
  }

  auto context()
  {
    return current;
  }

  void save_sketch(std::unique_ptr<Gate> sketch)
  {
    std::string name = sketch->name;
    components.insert({name, std::move(sketch)});
  }

  Gate* get_component(std::string_view name)
  {
    auto it = components.find(std::string(name));

    if (it != components.end())
    {
      return it->second.get();
    }

    return nullptr;
  }

  std::vector<std::string_view> get_names()
  {
    std::vector<std::string_view> res;

    for (const auto& [k, _] : components)
    {
      res.push_back(k);
    }

    return res;
  }

private:
  static Board*                                singleton;
  std::pair<std::string, Gate*>                current;
  std::map<std::string, std::unique_ptr<Gate>> components;
};

inline Board* Board::singleton = nullptr;

#endif /* BOARD */
