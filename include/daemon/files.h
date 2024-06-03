#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <fstream>

#include <nlohmann/json.hpp>

// for convenience
// using json = nlohmann::json;
using json = nlohmann::ordered_json;

struct FileHandler {
	FileHandler() = delete;
	FileHandler(const std::string &path);
	~FileHandler();

	// reads line, stores it into buffer. buffer is reset at the start. '\' makes it so that next character is ignored
	// no error checking is performed at all, other than returning false on EOF
	json readjson(); // make it so that buffer is internal to this class????


	std::ifstream file;
};

#endif
