#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>

#include "unistd.h"

// Symbol table for storing text
std::unordered_map<std::string, std::string> symbol_table;

using namespace std;

// Splits a string into a vector of strings using the specified delimiter
vector<string> split(const string& s, char delimiter) {
	vector<string> tokens;
	string token;
	istringstream token_stream(s);
	while (getline(token_stream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

// Returns true if the program exists, false otherwise
bool exists(const std::string& program) {
	// Check if the program is in the PATH
	if (getenv("PATH") == nullptr) {
		return false;
	}
	std::vector<std::string> paths = split(getenv("PATH"), ':');
	for (const std::string& path : paths) {
		if (access((path + "/" + program).c_str(), F_OK) == 0) {
			return true;
		}
	}
	return false;
}

int main() {
	while (true) {
		// get working directory
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != nullptr) {
			// Print the current working directory
			// std::cout << cwd;
		} else {
			perror("getcwd() error");
		}

		// Print the command prompt
		std::cout << cwd << " > ";

		// Read the command from standard input
		std::string command;
		std::getline(std::cin, command);

		// Variable for tracking if the command is an internal shell command and was already handled
		bool handled = false;

		// Expand tokens into the data stored at that token in the command
		std::regex token_regex("\\$\\w+");

		// Check thru entire command and if the any tokens exist in the symbol table, and if so, replace it in the string
		for (auto it = std::sregex_iterator(command.begin(), command.end(), token_regex); it != std::sregex_iterator(); ++it) {
			std::smatch match = *it;
			std::string token = match.str();
			if (symbol_table.count(token) > 0) {
				command = std::regex_replace(command, token_regex, symbol_table[token]);
			}
		}

		// Split the command into program and arguments
		std::vector<std::string> tokens = split(command, ' ');
		std::string program = tokens[0];
		std::vector<std::string> args(tokens.begin() + 1, tokens.end());

		// Handle the "cd" command
		if (program == "cd") {
			// Check if a directory was specified
			if (args.empty()) {
				std::cerr << "cd: missing operand" << std::endl;
				continue;
			}
			// Try to change the current directory
			if (chdir(args[0].c_str()) != 0) {
				perror("cd");
			}
			continue;

			handled = true;
		}

		// Handle the "exit" command
		if (program == "exit") {
			handled = true;
			break;
		}

		// Handle the "set" command for storing text in the symbol table
		if (program == "set") {
			// Check if a variable and value were specified
			if (args.size() < 2) {
				std::cerr << "set: missing operand" << std::endl;
				continue;
			}

			// Check if the variable name is valid
			if (args[0][0] == '$') {
				// Store the value in the symbol table
				symbol_table[args[0]] = args[1];
			} else {
				std::cerr << "set: missing operand" << std::endl;
			}
			continue;

			handled = true;
		}

		// Handle other commands
		// ...

		// Check if the command was handled by an internal shell command, and if so skip the rest of the loop
		if (handled) {
			break;
		}

		// Check if the user requested program exists before executing the command
		if (!exists(program)) {
			std::cerr << program << ": command not found" << std::endl;
			continue;
		}

		// Execute the command
		int status = system(command.c_str());

		// Check the exit status of the command
		if (status == 0) {
			// Command succeeded
			// ...
		} else if (status == 127) {
			// Command not found
			std::cerr << program << ": command not found" << std::endl;
		} else {
			// Command failed
			// ...
		}
	}

	return 0;
}
