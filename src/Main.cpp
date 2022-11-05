#include <fstream>
#include <string>
#include <iostream>
#include "../include/Lexer.hpp"
#include "../include/Exception.hpp"

std::string readFile(std::string_view filename)
{
	std::ifstream file{filename.data(), std::ios::ate};
	if (!file)
	{
		// TODO; Better error message.
		std::cerr << "Failed to open file " << filename.data() << '\n';
		std::exit(74); // I/O error
	}

	std::string file_contents;
	file_contents.resize(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(file_contents.data(), file_contents.size());
	file.close();
	return file_contents;
}

void run(std::string_view source)
{
	Lexer lexer{source};
	// std::vector<Token> tokens{lexer.scanTokens()};
	//  for (const Token token : tokens)
	//{
	//  std::cout << token << '\n';
	// }
}
void initFile(std::string_view filename)
{
	std::string file_contents{readFile(filename)};
	std::cout << file_contents << '\n';
	run(file_contents);
}

void runPrompt()
{
	std::cout << "type !exit to exit program." << '\n';
	while (true)
	{
		std::cout << "> ";
		std::string line;
		std::getline(std::cin, line);
		if (line == "!exit")
		{
			std::cout << "exiting program.";
			return;
		}
		run(line);
	}
}

int main(int argc, char *argv[])
{
	if (argc > 2)
		std::exit(64);
	else if (argc == 2)
		initFile(argv[1]);
	else
		runPrompt();
	return 0;
}
