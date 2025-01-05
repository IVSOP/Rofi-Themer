#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
// for convenience
using json = nlohmann::ordered_json;

using FileMode = std::ios_base::openmode;

// these are binary flags, just do A | B | C | ...
struct FileModes {
	static constexpr FileMode Bin = std::ios::binary;
	static constexpr FileMode Append = std::ios::app;
	static constexpr FileMode Trunc = std::ios::trunc | std::ios::out; // TODO: rethink if this 'out' should be here or be user's responsibility
	static constexpr FileMode Write = std::fstream::out;
	static constexpr FileMode Read = std::fstream::in;
};


struct FileHandler {
	FileHandler() = delete;
	FileHandler(const std::string &path);
	FileHandler(const std::string &path, FileMode flags);
	~FileHandler();

	// reads line, stores it into buffer. buffer is reset at the start. '\' makes it so that next character is ignored
	// no error checking is performed at all, other than returning false on EOF
	json readjson(); // make it so that buffer is internal to this class????
    void dumpjson(const json &json, uint32_t spacing);

	size_t write(void *data, size_t len);
	size_t read(void *buff, size_t len);


	std::fstream file;
};

#endif
