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

FileHandler::~FileHandler() {
    file.close();
}

json FileHandler::readjson() {
    return json::parse(this->file);
}
