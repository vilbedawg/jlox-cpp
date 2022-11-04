#include <fstream>
#include <string>
#include <iostream>
#include "Lexer/Lexer.h"

static bool had_error = false;

std::string readFile(const std::string_view filename)
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

void initFile(const std::string_view filename)
{
	std::string file_contents{readFile(filename)};
	std::cout << file_contents << '\n';
	// TODO; Pass the file_contents to the tokenizer
	// run(file_contents);
}

void run(std::string_view source)
{
	bis::Lexer lexer{source};
	std::vector<bis::Token> tokens;
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
		if (std::cin.bad() or std::cin.eof())
		{
			std::cout << '\n';
			return;
		}
		std::cout << line << '\n';
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
