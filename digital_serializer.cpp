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
				desc("P                 ", "Simulate the current component.");
				desc("C <component name>", "Create a new component.");
				desc("L                 ", "List all components");
				desc("S <component name>", "Set the current component as current");
				newline();
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
		break; case 'l':
					 case 'L':
		{
				for (auto n : Board::instance()->get_names())
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
				std::string name = std::string(str.substr(id+1));

				if (Board::instance()->found(name))
				{
						Board::instance()->set_context(name);
						log("Context switched, current: ", name, '\n');
						newline();
				}
				else
				{
					log("Component `", name, "` not found.");
					newline();
				}
			}
			else
			{
				info("Please provide a component name.");
				newline();
			}
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
