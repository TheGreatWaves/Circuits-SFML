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

#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <filesystem>
#include <optional>

#include "common.hpp" 
#include "board.hpp"
#include "gui/driver.hpp"

#include "lang/core/trie.hpp"
#include "lang/test/tester.hpp"
#include "lang/core/comptrie.hpp"
#include "lang/core/raw_parser.hpp"
#include "lang/assem/token_assem.hpp"
#include "lang/hdl/parser.hpp"

/**
 * Function prototypes.
 */
bool run_file(const std::string& filePath);
void create_component(std::string_view name);
void run_gui();

Board board;
bool running = true;

void greet()
{
	std::cout << BLOCK << " DIGITAL LOGIC " << BLOCK << '\n';
	info("Simulator starting...");
	info("`help` to show commands.");
	newline();
}

bool prompt()
{
	std::cout << "> ";
	return true;
}

void show_truth_table(RawParser& parser)
{
	auto board = Board::instance();
	const auto token = parser.advance_token();

	if (token.type != RawTokenType::Identifier && !token.type.is_keyword())
	{
		error("Please input a valid component name.");
		return;
	}

	// Get the component name.
	const auto& name = token.lexeme;

	if (auto component = board->get_component(name); component != nullptr)
	{
		component->print_truth_table();
	}
	else
	{
		log("Component with given name `", name, "` not found!");
	}
}

void show_list(RawParser& parser)
{
	auto component_names = Board::instance()->get_names();
	if (component_names.empty())
	{
		info("No component found.");
		return;
	}
	for (const auto& n : component_names)
	{
		log(n);
	}
}

void compile(RawParser& parser)
{
	const auto token = parser.advance_token();

	if (token.type != RawTokenType::Identifier && !token.type.is_keyword())
	{
		error("Please input a valid component name.");
		return;
	}

	// Get the component name.
	const auto& name = token.lexeme;

	auto hdl_parser = hdl::HDLParser(hdl_file(name));

	if (hdl_parser.error_occured())
	{
		error("Failed to open file '" + name + "'.");
		return;
	}

	if (!hdl_parser.parse())
	{
		error("Failed to parse file '" + name + "'.");
		return;
	}

	const auto result = hdl_parser.result();

	// Gate file.
  std::ofstream gate_file { 
    DEFAULT_GATE_DIRECTORY + 
    std::string("/") +  
    DEFAULT_RECIPE_SAVE_DIRECTORY + 
    std::string("/") +  
    name + 
    GATE_EXTENSION 
  };
	gate_file << result.compile();
	gate_file.close();

	// Meta file.
  std::ofstream meta_file { 
    SCRIPTS_DIR + 
    std::string("/") +  
    name + 
    META_EXTENSION 
  };
	meta_file << result.meta();
	meta_file.close();


	// Attemp to load the chip.
	if (!run_file(GATE_RECIPE_DIRECTORY + name + GATE_EXTENSION))
	{
		error("Failed to load file '" + name + "'.");
		return;
	}

	log("Successfully compiled and loaded '", name, "'.");
}

void load(RawParser& parser)
{
	const auto token = parser.advance_token();

	if (token.type != RawTokenType::Identifier && !token.type.is_keyword())
	{
		error("Please input a valid component name.");
		return;
	}

	// Get the component name.
	const std::string& name = token.lexeme;

	const auto component_names = Board::instance()->get_names();
	auto found = std::find(component_names.begin(), component_names.end(), name);

	if (found != component_names.end())
	{
		log("Chip '", name, "' already loaded.");
		return;
	}

	// Attemp to load the chip.
	if (!run_file(GATE_RECIPE_DIRECTORY + name + GATE_EXTENSION))
	{
		error("Failed to load file '" + name + "'.");
		return;
	}

	log("Sucessfully loaded '", name, "'.");
}

void run_test(RawParser& parser) 
{
	const auto token = parser.advance_token();

	if (token.type != RawTokenType::Identifier && !token.type.is_keyword())
	{
		error("Please input a valid component name.");
		return;
	}

	// Get the component name.
	const std::string& name = token.lexeme;

	
	test::Tester tester(SCRIPTS_DIR + SEPERATOR + name + TEST_EXTENSION);
	if (!tester.parse())
	{
		std::cout << "Something went very wrong!\n";
	}
}

void handle_input(RawParser& parser, std::string_view str)
{
	parser.set_source(std::string(str));
	static_cast<void>(parser.advance_token());

	MATCH(parser.get_current().lexeme)
		info("Invalid command. Try 'help'.");
	CASE("compile")
		compile(parser);
	CASE("help")
		desc("gui               ", "Start GUI mode.");
		desc("list              ", "List all components.");
		desc("info              ", "Display general information.");
		desc("test        <chip>", "Run test file.");
		desc("load        <chip>", "Load the specified chip.");
		desc("compile     <file>", "Compile the hdl file with the given name.");
	CASE("info")
		log("Gate Recipe Directory: ", GATE_RECIPE_DIRECTORY);
	CASE("test")
		run_test(parser);
	CASE("gui")
		run_gui();
	CASE("quit")
		running = false;
	CASE("list")
		show_list(parser);
	CASE("load")
		load(parser);
  ENDMATCH;

	// switch (command.at(0))
	// {
	// 	break; case 'H':
	// 				 case 'h':
	// 	{
	// 			desc("R                 ", "Simulate the current component.");
	// 			desc("C <component_name>", "Create a new component.");
	// 			desc("L                 ", "List all components.");
	// 			desc("P                 ", "List all current components.");
	// 			desc("S <component_name>", "Set the current component as current.");
	// 			desc("A <component_name>", "Add the specified component to current configuration.");
	// 			desc("T     <pin_number>", "Toggle the pin specified.");
	// 			desc("I            <+/->", "Add/Delete input pin.");
	// 			desc("O            <+/->", "Add/Delete output pin.");
	// 			desc("D                 ", "Dump current component information.");	
	// 			desc("W     <src> <dest>", "Wire source pin and destination pin.");
	// 			desc("W     <src> <dest>", "Wire source pin and destination pin.");
	// 			desc("E <component_name>", "Serialize the current component.");
	// 			desc("F    <source_file>", "Run file");
	// 			desc("Q <component_name>", "Show truth table of component.");
	// 			desc("N <component_name>", "Marks specified component as needed, auto import if available.");
	// 			desc("X                 ", "Reset context.");
	// 			desc("G                 ", "Start GUI mode.");
	// 	}
	// 	break; case 'x':
	// 	{
	// 		Board::instance()->reset_context();
	// 	}
	// 	break; case 'n':
	// 	{
	// 		auto board = Board::instance();
			
	// 		if (auto id = str.find(" "); id < str.size() - 1)
	// 		{
	// 			std::string name = std::string(str.substr(id+1));

	// 			if (!board->found(name))
	// 			{
	// 				if (!run_file(GATE_RECIPE_DIRECTORY + name + GATE_EXTENSION))
	// 				{
	// 					log("Error: Needed component with given name `", name, "` not found!\n");
	// 					exit(1);
	// 				}
	// 			}
	// 		}			
	// 		else
	// 		{
	// 				log("Error: Please specify needed component\n");
	// 		}
	// 	}
	// 	break; case 'q':
	// 	{
	// 		auto board = Board::instance();
			
	// 		if (auto id = str.find(" "); id < str.size() - 1)
	// 		{
	// 			std::string name = std::string(str.substr(id+1));

	// 			if (auto component = board->get_component(name); component != nullptr)
	// 			{
	// 				component->print_truth_table();
	// 			}
	// 			else
	// 			{
	// 				log("Component with given name `", name, "` not found!\n");
	// 			}
	// 		}			
	// 		else
	// 		{
	// 			// Print the truth table of the current component
	// 			auto current = board->context().second;

	// 			if (current != nullptr)
	// 			{
	// 				current->print_truth_table();
	// 			}
	// 			else
	// 			{
	// 				log("Current context is empty, please select a configuration\n");
	// 			}
	// 		}
	// 	}
	// 	break; case 'f':
	// 	{
	// 		if (auto id = str.find(" "); id < str.size() - 1)
	// 		{
	// 			auto file = std::string(str.substr(id+1));
	// 			log("Running file ", file, "\n");
	// 			run_file(file);
	// 			log("Finished running file.\n");
	// 		}
	// 		else
	// 		{
	// 			log("Please specify a file.\n");
	// 		}
	// 	}
	// 	break; case 'e':
	// 	{
	// 		auto board = Board::instance();
	// 		auto current = board->context().second;
	// 		/**
	// 		 * Use the current context.
	// 		 */
	// 		if (current == nullptr)
	// 		{
	// 			log("Current context is empty, please select a configuration\n");
	// 			return;
	// 		}
	// 		current->serialize();
	// 		current->print_truth_table();
	// 	}
	// 	break; case 'w':
	// 	{
	// 		auto board = Board::instance();
	// 		auto current = board->context().second;
	// 		std::stringstream ss {command};

	// 		if (current == nullptr)
	// 		{
	// 			log("Current context is empty, please select a configuration\n");
	// 			return;
	// 		}

	// 		std::string c;
	// 		std::size_t p1, p2;

	// 		ss >> c;
	// 		ss >> p1;
	// 		ss >> p2;

	// 		if (!current->wire_pins(p1, p2))
	// 		{
	// 			log("Failed to wire pin ", p1, " and ", p2, '\n');
	// 		}
	// 		else
	// 		{
	// 			log("Successfully wired pin ", p1, " and ", p2, '\n');
	// 		}
	// 	}
	// 	break; case 'd':
	// 	{
	// 		auto board = Board::instance();
	// 		auto current = board->context().second;
	// 		if (current == nullptr)
	// 		{
	// 			log("Current context is empty, please select a configuration\n");
	// 			return;
	// 		}

	// 		current->info();
	// 	}
	// 	break; case 'R':
	// 				 case 'r':
	// 	{
	// 			auto board = Board::instance();
	// 			auto current = board->context().second;
	// 			if (current == nullptr)
	// 			{
	// 				log("Current context is empty, please select a configuration\n");
	// 				return;
	// 			}
	// 			log("=== Preparing ===\n");

	// 			current->reset();

	// 			newline();
	// 			current->simulate();

	// 			log("=== Output pins ===\n");

	// 			auto count = 0;
	// 			for (const auto& pin : current->output_pins)
	// 			{
	// 				log("pin[", count, "] ", pin.state == PinState::ACTIVE ? 1 : 0, "\n");
	// 				count++;
	// 			}
	// 	}
	// 	break; case 'I':
	// 				 case 'i':
	// 				 case 'o':
	// 				 case 'O':
	// 	{
	// 			auto board = Board::instance();
	// 			auto current = board->context().second;
	// 			if (current == nullptr)
	// 			{
	// 				log("Current context is empty, please select a configuration\n");
	// 				return;
	// 			}

	// 			if (auto id = str.find(" "); id < str.size() - 1)
	// 			{
	// 				auto symbol = std::string(str.substr(id+1));

	// 				if (symbol == "+")
	// 				{

	// 					if (command[0] == 'i')
	// 					{
	// 						info("Input pin added.");
	// 						current->add_input_pin();
	// 					}
	// 					else 
	// 					{
	// 						info("Output pin added.");
	// 						current->add_output_pin();
	// 					}
	// 				}
	// 				else if (symbol == "-")
	// 				{
	// 					// TODO: Implement this later...
	// 				}

	// 			}				
	// 			else
	// 			{
	// 				info("Please specify action symbol.");
	// 			}
	// 	}
	// 	break; case 'T':
	// 				 case 't':
	// 	{
	// 			auto board = Board::instance();
	// 			auto current = board->context().second;
	// 			if (current == nullptr)
	// 			{
	// 				log("Current context is empty, please select a configuration\n");
	// 				return;
	// 			}

	// 		std::string delimiter = " ";

	// 		if (auto id = str.find(delimiter); id < str.size() - 1)
	// 		{
	// 			auto pin_id = static_cast<std::size_t>(std::stoi(std::string(str.substr(id+1))));

	// 			if (current->has_pin(pin_id) && current->toggle_pin(pin_id))
	// 			{
	// 				log("Toggled pin ", pin_id, " to ", (current->get_pin_state(pin_id) == PinState::ACTIVE ? "active" : "inactive"), ".\n");
	// 			}
	// 			else
	// 			{
	// 				info("Pin ID invalid.");
	// 			}
	// 		}
	// 		else
	// 		{
	// 			info("Please specify pin ID.");
	// 		}
	// 	}
	// 	break; case 'P':
	// 				 case 'p':
	// 	{
	// 			auto board = Board::instance();
	// 			auto current = board->context();
	// 			if (current.second == nullptr)
	// 			{
	// 				log("Current context is empty, please select a configuration\n");
	// 				return;
	// 			}

	// 			log(current.first);
	// 			newline();

	// 			for (auto& c : current.second->subgates)
	// 			{
	// 				log(c->name);
	// 				newline();
	// 			}

	// 	}
	// 	break; case 'C':
	// 				 case 'c':
	// 	{
	// 		std::string delimiter = " ";

	// 		if (auto id = str.find(delimiter); id < str.size() - 1)
	// 		{
	// 			std::string name = std::string(str.substr(id+1));
	// 			create_component(name);
	// 		}
	// 		else
	// 		{
	// 			info("Please provide a component name.");
	// 		}
	// 	}
	// 	break; case 'A':
	// 				 case 'a':
	// 	{
	// 		auto board = Board::instance();
	// 		auto current = board->context();

	// 		if (current.second == nullptr)
	// 		{
	// 			log("Current context is empty, please select a configuration\n");
	// 			return;
	// 		}

	// 		if (auto id = str.find(" "); id < str.size() - 1)
	// 		{
	// 			std::string name = std::string(str.substr(id+1));

	// 			if (auto component = board->get_component(name); component != nullptr)
	// 			{
	// 				current.second->add_subgate(component);
	// 			}
	// 			else
	// 			{
	// 				log("Component with given name `", name, "` not found!\n");
	// 			}
	// 		}
	// 		else
	// 		{
	// 			info("Please provide a component name.\n");
	// 		}
	// 	}
	// 	break; case 'l':
	// 				 case 'L':
	// 	{
	// 			auto component_names = Board::instance()->get_names();

 // 				if (component_names.empty())
	// 			{
	// 				info("No component found.");
	// 				return;
	// 			}
 
	// 			for (auto n : component_names)
	// 			{
	// 				log(n, '\n');
	// 			}
	// 	}
	// 	break; case 's':
	// 				 case 'S':
	// 	{
	// 		std::string delimiter = " ";

	// 		if (auto id = str.find(delimiter); id < str.size() - 1)
	// 		{
	// 			std::string name = std::string(str.substr(id+1));

	// 			auto board = Board::instance();

	// 			if (board->found(name))
	// 			{
	// 					if (board->get_component(name)->type != GateType::CUSTOM)
	// 					{
	// 						log("Gate type `", name, "` is built in, getting context is forbidden.\n");
	// 					}
	// 					else
	// 					{
	// 						board->set_context(name);
	// 						log("Context switched, current: ", name, '\n');
	// 					}
	// 			}
	// 			else
	// 			{
	// 				log("Component `", name, "` not found.\n");
	// 			}
	// 		}
	// 		else
	// 		{
	// 			info("Please provide a component name.");
	// 		}
	// 	}
	// 	break; default:
	// 	{
	// 			info("Invalid command.");
	// 	}
	// }
}

void init()
{
	const auto gate_sketch_dir = DEFAULT_GATE_DIRECTORY + std::string("/") + DEFAULT_RECIPE_SAVE_DIRECTORY;

	std::filesystem::create_directory(DEFAULT_GATE_DIRECTORY);
	std::filesystem::create_directory(gate_sketch_dir);
	std::filesystem::create_directory(SCRIPTS_DIR);

	// Load init files.
	auto init_file = DEFAULT_GATE_DIRECTORY + std::string("/init") + GATE_EXTENSION;
	auto _ = std::ofstream { init_file  };
	run_file( init_file  );

	// Load sketches. (Temporary)
	// using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

	// for (const auto& gate : std::filesystem::directory_iterator(gate_sketch_dir))
 //  {
	// 	if (gate.path().extension() == GATE_EXTENSION)
	// 	{
	// 		run_file(gate.path());
	// 	}
	// }
}


void run_cli()
{
	std::string line;

	greet();

	RawParser parser;	

	while(running && prompt() && std::getline(std::cin, line))
	{
		if (line.empty()) continue;
		handle_input(parser, line);
		newline();
	}
}

bool run_file(const std::string& file_path)
{
	auto instance = Board::instance();
	return instance->load_file(file_path);
}

void create_component(std::string_view name)
{
	auto instance = Board::instance();

	instance->create_new(name);
	instance->set_context(name);
}

void run_gui()
{
	Program program;
	program.run();
}

int main()
{
	init();
	run_cli();
	// Trie trie;
	// trie.insert("hello");
	// trie.insert("hello bob");
	// trie.insert("hello joe");
	// trie.insert("hello lol");

	// for (const auto v : trie.fuzzy("hello"))
	// {
	// 	std::cout << "Value: " << v << '\n';
	// }
}
