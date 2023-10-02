#pragma once
#ifndef BOARD
#define BOARD

#include "digital_serializer.hpp"
#include "utils.hpp"
#include <memory>
#include <map>

// We have a single static instance of the board (this will have the lifetime of the program)
class Board
{
public:
  Board()
  {
    auto andc = std::make_unique<Gate>(2, 1, GateType::AND, "and", true);
    auto andp = andc.get();
    components["and"] = std::move(andc);
    components["and"]->get_pin(0)->parent = andp;
    components["and"]->get_pin(1)->parent = andp;

    auto notc = std::make_unique<Gate>(1, 1, GateType::NOT, "not", true);
    auto notp = notc.get();

    components["not"] = std::move(notc);
    components["not"]->get_pin(0)->parent = notp;

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
    components[make_lower(name)] = std::make_unique<Gate>();
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

  Gate* get_component(std::string_view name)
  {
    if (found(name))
    {
      return components[std::string(name)].get();
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
