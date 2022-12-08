#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// split a string into a vector of strings using the specified delimiter
vector<string> split(const string& s, char delimiter) {
	vector<string> tokens;
	string token;
	istringstream token_stream(s);
	while (getline(token_stream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

int processCommand(char* args[64], vector<string> command_args) {
	// convert the command and its arguments to an array of c-strings
	for (int i = 0; i < command_args.size(); i++) {
		args[i] = &command_args[i][0];
	}

	// add a null pointer to the end of the array
	args[command_args.size()] = NULL;

	// execute the command
	execvp(args[0], args);

	return 0;
}

int main() {
	// the current working directory
	string cwd;

	// the prompt to display
	string prompt;

	// the command entered by the user
	string command;

	// the command and its arguments as a vector of strings
	vector<string> command_args;

	// the command and its arguments as an array of c-strings
	char* args[64];

	// continuously prompt the user for input
	while (true) {
		// get the current working directory
		cwd = getcwd(NULL, 0);

		// construct the prompt string
		prompt = cwd + " $ ";

		// print the prompt and read the command from the user
		cout << prompt;
		getline(cin, command);

		// check for empty command and skip the rest of the program if it is
		if (command.empty()) {
			break;
		} else {
			continue;
		}

		// split the command into individual arguments
		command_args = split(command, ' ');

		if (command_args[0] == "exit") {
			break;
		}

		if (command_args[0] == "cd") {
			// change the current working directory to home if the user did not specify a directory
			if (command_args[1].empty()) {
				chdir(getenv("HOME"));
				continue;
			} else {
				chdir(command_args[1].c_str());
			}
			continue;
		}

		if (command_args[0] == "pwd") {
			cout << cwd << endl;
			continue;
		}

		processCommand(args, command_args);
	}

	return 0;
}