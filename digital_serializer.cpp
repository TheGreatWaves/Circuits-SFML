#include <iostream>
#include <string>
#include <string_view>

#include "board.hpp"

#define BLOCK "======"

/**
 * Logging related functions.
 */
template<typename ... Args> auto log(Args&& ... args)->void{(std::cout<<...<<std::forward<Args>(args));} 

void desc(std::string_view name, std::string_view brief)
{
	std::cout << name << ": " << brief << '\n';
}

void newline()
{
	std::cout << '\n';
}

void info(std::string_view dump)
{
	std::cout << dump << '\n';
}

void greet()
{
	std::cout << BLOCK << " DIGITAL LOGIC " << BLOCK << '\n';
	info("Simulator starting...");
	info("`h` for help.");
	newline();
}

bool prompt()
{
	std::cout << "> ";
	return true;
}

void create_component(std::string_view name)
{
	auto instance = Board::instance();

	instance->create_new(name);
	instance->set_context(name);

	log("New component created: ", name, ". Context switched.");
	newline();
	newline();
}

void handle_input(std::string_view str)
{
	if (str.empty()) return;

	switch (str.at(0))
	{
		break; case 'H':
					 case 'h':
		{
				desc("R                 ", "Simulate the current component.");
				desc("C <component name>", "Create a new component.");
				desc("L                 ", "List all components.");
				desc("S <component name>", "Set the current component as current.");
				desc("A <component_name>", "Add the specified component to current configuration.");
				newline();
		}
		break; case 'R':
					 case 'r':
		{
				auto board = Board::instance();
				auto current = board->context().second;
				current->simulate();
		}
		break; case 'C':
					 case 'c':
		{
			std::string delimiter = " ";

			if (auto id = str.find(delimiter); id < str.size() - 1)
			{
				std::string name = std::string(str.substr(id+1));
				create_component(name);
			}
			else
			{
				info("Please provide a component name.");
				newline();
			}
		}
		break; case 'A':
					 case 'a':
		{
			auto board = Board::instance();
			auto current = board->context();

			if (current.second == nullptr)
			{
				log("Current context is empty, please select a configuration\n");
				newline();
				return;
			}

			if (auto id = str.find(" "); id < str.size() - 1)
			{
				std::string name = make_lower(std::string(str.substr(id+1)));

				if (auto component = board->get_component(name); component != nullptr)
				{
					auto id = current.second->add_subgate(component);

					log("Component successfully added with ID ", id, "\n");
				}
				else
				{
					log("Component with given name `", name, "` not found!\n");
				}
			}
			else
			{
				info("Please provide a component name.\n");
			}
			newline();
		}
		break; case 'l':
					 case 'L':
		{
				auto component_names = Board::instance()->get_names();

 				if (component_names.empty())
				{
					info("No component found.");
					newline();
					return;
				}
 
				for (auto n : component_names)
				{
					log(n, '\n');
				}
				newline();
		}
		break; case 's':
					 case 'S':
		{
			std::string delimiter = " ";

			if (auto id = str.find(delimiter); id < str.size() - 1)
			{
				std::string name = make_lower(std::string(str.substr(id+1)));

				auto board = Board::instance();

				if (board->found(name))
				{
						if (board->get_component(name)->type != GateType::CUSTOM)
						{
							log("Gate type `", name, "` is built in, getting context is forbidden.\n");
						}
						else
						{
							board->set_context(name);
							log("Context switched, current: ", name, '\n');
						}
				}
				else
				{
					log("Component `", name, "` not found.\n");
				}
			}
			else
			{
				info("Please provide a component name.");
			}
			newline();
		}
		break; default:
		{
				info("Invalid command.");
				newline();
		}
	}
}

int main()
{
	Board board;

	std::string line;

	greet();

	while(prompt() && std::getline(std::cin, line))
	{
		handle_input(line);
	}
}
