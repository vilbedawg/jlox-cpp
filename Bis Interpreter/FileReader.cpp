#include <fstream>
#include <string>
#include <iostream>

namespace reader
{
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


	void runFile(const std::string_view filename)
	{
		std::string file_contents{ readFile(filename) };

		// TODO; Pass the file_contents to the tokenizer

	}
}
