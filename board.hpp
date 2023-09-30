#pragma once
#ifndef BOARD
#define BOARD

#include "digital_serializer.hpp"
#include <memory>
#include <map>

// We have a single static instance of the board (this will have the lifetime of the program)
class Board
{
public:
  Board()
  {
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
    // Default is two input and one output.
    components[std::string(name)] = std::make_unique<Gate>(2, 1);
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

  std::vector<std::string_view> get_names()
  {
    std::vector<std::string_view> res;

    for (auto& [k, _] : components)
    {
      res.push_back(k);
    }

    return res;
  }

private:

  static Board*                                singleton;

  std::pair<std::string, Gate*>                                        current;
  std::map<std::string, std::unique_ptr<Gate>> components;
};

inline Board* Board::singleton = nullptr;

#endif /* BOARD */
