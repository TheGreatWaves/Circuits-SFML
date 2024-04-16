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

#include <memory>
#include <map>

#include "lang/assem/token_assem.hpp"
#include "lang/core/trie.hpp"
#include "lang/hdl/parser.hpp"
#include "gate.hpp"
#include "utils.hpp"

// We have a single static instance of the board (this will have the lifetime of the program)
class Board
{
public:
  Board(bool is_singleton = true)
   : is_singleton(is_singleton)
  {
    auto nandc = std::make_unique<Gate>(2, 1, GateType::NAND, "nand", true);
    auto nandp = nandc.get();
    components["nand"] = std::move(nandc);
    auto nand = components["nand"].get();
    nand->get_pin(0)->parent = nandp;
    nand->get_pin(1)->parent = nandp;
    nand->serialize();
    search_trie.insert("nand");

    if (is_singleton)
    {
      singleton = this;
    }
  }

  ~Board()
  {
    if (is_singleton)
    {
      singleton = nullptr;
    }
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
    if (!current.first.empty())
    {
      search_trie.insert(current.first);
    }
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

  std::vector<Gate*> search(const std::string& name) 
  {
      std::vector<Gate*> res;
      const auto entries = search_trie.fuzzy(name);

      for (const auto& entry : entries)
      {
        res.push_back(get_component(entry));
      }

      return res;
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
        auto p = it->second.get();
        return p;
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

  bool load_file(const std::string& file_path)
  {
    AssemTokenTypeScanner scanner{};

  	if (!scanner.read_source(std::string(file_path)))
  	{
  		return false;
  	}

  	auto board = this;

  	while (!scanner.is_at_end())
  	{
  		const auto token = scanner.scan_token();		

  		switch(token.type)
  		{
  			break; case AssemTokenType::EndOfFile: {}
  			break; case AssemTokenType::Number: 
  			       case AssemTokenType::Identifier:
  			       case AssemTokenType::Error:
  			{ 
  				return false; 
  			}
        break; case AssemTokenType::Need: 
  			{
  				const auto next = scanner.scan_token();
  				const auto chip_name = next.lexeme;

  				if (next.type != AssemTokenType::Identifier)
  				{
  					log("Error: Invalid argument to need, not an identifier.");
  					return false;
  				}
				
  				if (!board->found(chip_name) && !load_file(GATE_RECIPE_DIRECTORY + chip_name + GATE_EXTENSION))
  				{
          	auto hdl_parser = hdl::HDLParser(hdl_file(chip_name));
            
          	if (hdl_parser.error_occured())
          	{
          		error("Failed to open file '" + chip_name + "'.");
          		return false;
          	}

            const bool success = hdl_parser.parse();

          	if (!success)
          	{
    					log("Error: Needed component with given name `", chip_name, "` not found!");
    					exit(1);
          	}

          	const auto result = hdl_parser.result();

          	// Gate file.
            std::ofstream gate_file { 
              DEFAULT_GATE_DIRECTORY + 
              std::string("/") +  
              DEFAULT_RECIPE_SAVE_DIRECTORY + 
              std::string("/") +  
              chip_name + 
              GATE_EXTENSION 
            };
          	gate_file << result.compile();
          	gate_file.close();

          	// Meta file.
            std::ofstream meta_file { 
              SCRIPTS_DIR + 
              std::string("/") +  
              chip_name + 
              META_EXTENSION 
            };
          	meta_file << result.meta();
          	meta_file.close();

            if (!load_file(GATE_RECIPE_DIRECTORY + chip_name + GATE_EXTENSION))
            {
    					log("Error: Failed to load component with given name `", chip_name, "`.");
    					exit(1);
            }
  				}
  			}
        break; case AssemTokenType::Precompute:
        {
  				auto current = board->context().second;		

  				if (current == nullptr)
  				{
  					error("Current context is empty, please select a configuration");
  					return false;
  				}

          current->serialize();
        }
        break; case AssemTokenType::Create: 
  			{
  				const auto next = scanner.scan_token();
  				const auto name = next.lexeme;

  				if (next.type != AssemTokenType::Identifier && !next.type.is_keyword())
  				{
  					log("Error: Please provide a valid component name to create.");
  					return false;	
  				}

        	board->create_new(name);
        	board->set_context(name);

  				if (board->context().second == nullptr)
  				{
  					log("Create: Error: Failed to create context.");
  				}

  			}
        break; case AssemTokenType::In: 
  			{
  				auto current = board->context().second;

  				if (current == nullptr)
  				{
  					log("IN: Error: Current context is empty, please select a configuration.");
  					return false;
  				}

  				const auto next = scanner.scan_token();

  				if (next.type != AssemTokenType::Number)
  				{
  					error("Input count is not a number.");
  					return false;
  				}

  				current->add_input_pin(std::stoi(next.lexeme));
  			}
        break; case AssemTokenType::Out:
  			{
  				auto current = board->context().second;

  				if (current == nullptr)
  				{
  					log("OUT: Error: Current context is empty, please select a configuration.");
  					return false;
  				}

  				const auto next = scanner.scan_token();

  				if (next.type != AssemTokenType::Number)
  				{
  					error("Output count is not a number.");
  					return false;
  				}

  				current->add_output_pin(std::stoi(next.lexeme));
  			}
        break; case AssemTokenType::Add: 
  			{
  				auto current = board->context();

  				if (current.second == nullptr)
  				{
  					error("Add: Current context is empty, please select a configuration\n");
  					return false;
  				}

  				const auto next = scanner.scan_token();
  				const auto chip_name = next.lexeme;

  				if (auto component = board->get_component(chip_name); component != nullptr)
  				{
  					current.second->add_subgate(component, this);
  				}
  				else
  				{
  					error("Component with given name `" + chip_name + "` not found!");
  					return false;
  				}
  			}
        break; case AssemTokenType::Wire: 
  			{
  				auto current = board->context().second;		

  				if (current == nullptr)
  				{
  					error("Current context is empty, please select a configuration");
  					return false;
  				}

  				auto pin1 = scanner.scan_token();
  				auto pin2 = scanner.scan_token();

  				if (pin1.type != AssemTokenType::Number 
  				&& pin2.type != AssemTokenType::Number)
  				{
  					error("Pin 1/2 not a number.");
  					return false;
  				}

  				if (!current->wire_pins(std::stoi(pin1.lexeme), std::stoi(pin2.lexeme)))
  				{
  					error("Failed to wire pin " + pin1.lexeme + " and pin " + pin2.lexeme);
  					return false;
  				}
  			}
        break; case AssemTokenType::Save: 
  			{
  				// Simply get out of context.
  				board->reset_context();
  			}
      }
  	}

  	return true;
  }

private:
  Trie                                         search_trie;
  static Board*                                singleton;
  std::pair<std::string, Gate*>                current;
  std::map<std::string, std::unique_ptr<Gate>> components;
  bool                                         is_singleton = false;
};

inline Board* Board::singleton = nullptr;

#endif /* BOARD */
