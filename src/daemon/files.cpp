#include "files.h"
#include "errors.h"

FileHandler::FileHandler(const std::string &path)
: file(path)
{
    if (!file.is_open()) {
        print_error(("Error opening file" + path).c_str());
        exit(1);
    }
}

FileHandler::FileHandler(const std::string &path, FileMode flags)
: file(path, flags)
{
	if (!file.is_open()) {
        print_error(("Error opening file: " + path).c_str());
        exit(1);
    }
}

FileHandler::~FileHandler() {
    file.close();
}

json FileHandler::readjson() {
    return json::parse(this->file);
}

void FileHandler::dumpjson(const json &json, uint32_t spacing) {
    file << json.dump(spacing);
}

// TODO these are awfull, terrible even
// need to catch all exceptions here and crash if needed
size_t FileHandler::write(void *data, size_t len) {
    const size_t start = file.tellp();
    file.write(reinterpret_cast<char *>(data), len);
    const size_t end = file.tellp();
    return end - start;
}

size_t FileHandler::read(void *buff, size_t len) {
    file.read(reinterpret_cast<char *>(buff), len);
    return file.gcount();
}

