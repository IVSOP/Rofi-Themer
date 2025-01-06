#include "files.hpp"
#include "crash.hpp"

FileHandler::FileHandler(const std::string &path)
: file(path)
{
    CRASH_IF(!file.is_open(), "Error opening file " + path);
}

FileHandler::FileHandler(const std::string &path, FileMode flags)
: file(path, flags)
{
    CRASH_IF(!file.is_open(), "Error opening file " + path);
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

