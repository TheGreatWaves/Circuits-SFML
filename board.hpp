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
    components["nand"]->get_pin(0)->parent = nandp;
    components["nand"]->get_pin(1)->parent = nandp;

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
    bool f = found(name);
    std::string entry{ name };
    current = f ? std::pair(name, components[entry].get()) : std::pair{"", nullptr};
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
