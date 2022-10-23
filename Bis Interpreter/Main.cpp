#include <fstream>
#include <string>
#include <iostream>

std::string readFile(const std::string_view filename)
{
	std::ifstream file{ filename.data(), std::ios::ate };
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
	std::string file_contents{ readFile(filename) };
	std::cout << file_contents << '\n';
	// TODO; Pass the file_contents to the tokenizer
    // run(file_contents);
}

void run(std::string_view source) {
//    Scanner scanner {source};
//    std::vector<Token> tokens = scanner.scanTokens();
//
//    // For now, just print the tokens.
//    for (const Token& token : tokens) {
//        std::cout << token.toString() << "\n";
//    }
}

void runPrompt() {
    while (true) {
        std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line)) break;
        std::cout << line;
        // TODO; Pass the file_contents to the tokenizer
        // run(line);
    }
}

int main(int argc, char* argv[])
{
    if (argc > 2) {
        std::exit(64);
    } else if (argc == 2) {
        initFile(argv[1]);
    } else {
        runPrompt();
    }

    return 0;
}