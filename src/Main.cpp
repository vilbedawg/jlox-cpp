#include "../include/Error_handler.hpp"
#include "../include/Lexer.hpp"
#include <fstream>

std::string readFile(std::string_view filename)
{
    std::ifstream file{filename.data(), std::ios::ate};
    if (!file)
    {
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
    std::vector<Token> tokens{lexer.scanTokens()};
    for (const Token& token : tokens)
    {
        std::cout << token.toString() << '\n';
    }
}

void initFile(std::string_view filename)
{
    std::string file_contents{readFile(filename)};
    std::cout << file_contents << '\n';
    run(file_contents);
}

void runPrompt()
{
    while (true)
    {
        std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line))
            return;

        run(line);
        if (Error::hadError)
            std::exit(65);
    }
}

int main(int argc, char* argv[])
{
    if (argc > 2)
        std::exit(64);
    else if (argc == 2)
        initFile(argv[1]);
    else
        runPrompt();
    return 0;
}