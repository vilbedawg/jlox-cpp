#include "../include/AstPrinter.hpp"
#include "../include/Interpreter.hpp"
#include "../include/Lexer.hpp"
#include "../include/Logger.hpp"
#include "../include/Parser.hpp"

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
    Interpreter interpreter;
    Lexer lexer{source};
    Parser parser{lexer.scanTokens()};
    interpreter.interpret(parser.parse());
    if (Error::hadError || Error::hadRuntimeError)
    {
        Error::report();
        return;
    }

    // AstPrinter printer;
    // std::cout << printer.print(expressions);
}

void initFile(std::string_view filename)
{
    std::string file_contents = readFile(filename);
    run(file_contents);
    if (Error::hadError)
    {
        std::exit(65);
    }
    if (Error::hadRuntimeError)
    {
        std::exit(70);
    }
}

void runPrompt()
{
    while (true)
    {
        std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line))
        {
            return;
        }

        run(line);
        if (Error::hadError)
        {
            std::exit(65);
        }
        if (Error::hadRuntimeError)
        {
            std::exit(70);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        std::exit(64);
    }
    else if (argc == 2)
    {
        initFile(argv[1]);
    }
    else
    {
        runPrompt();
    }

    return 0;
}
