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

void compile(RawParser& parser);

void compile_all()
{
	RawParser p;
	for (const auto& gate : std::filesystem::directory_iterator(SCRIPTS_DIR))
  {
		if (gate.path().extension() == HDL_EXTENSION)
		{
			p.set_source(gate.path().stem());
			compile(p);
		}
	}
}

void compile(RawParser& parser)
{
	const auto token = parser.advance_token();

	if (token.type == RawTokenType::Identifier && token.lexeme == "all")
	{
		compile_all();
		return;
	}

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

void run_all_tests()
{
	for (const auto& gate : std::filesystem::directory_iterator(SCRIPTS_DIR))
  {
		if (gate.path().extension() == TEST_EXTENSION)
		{
			log("Test file: " + std::string(gate.path()));
			test::Tester tester(gate.path());
			static_cast<void>(tester.parse());
		}
	}
}

void run_test(RawParser& parser) 
{
	const auto token = parser.advance_token();

	if (token.type == RawTokenType::Identifier && token.lexeme == "all")
	{
		run_all_tests();
		return;
	}

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

void serialize(RawParser& parser) 
{
	auto board = Board::instance();
	const auto token = parser.advance_token();

	if (token.type != RawTokenType::Identifier)
	{
		error("Please input a valid component name.");
		return;
	}

	// Get the component name.
	const auto& name = token.lexeme;

	if (auto component = board->get_component(name); component != nullptr)
	{
		component->serialize();
		// component->print_truth_table();
		log("Component `", name, "` serialized!");
	}
	else
	{
		log("Component with given name `", name, "` not found!");
	}}

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
	CASE("serialize")
		serialize(parser);
	CASE("list")
		show_list(parser);
	CASE("load")
		load(parser);
  ENDMATCH;
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

	// Load sketches.
	using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

	for (const auto& gate : std::filesystem::directory_iterator(gate_sketch_dir))
  {
		if (gate.path().extension() == GATE_EXTENSION)
		{
			run_file(gate.path());
		}
	}
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
}
