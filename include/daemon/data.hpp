#ifndef DATA_H
#define DATA_H

#include "files.hpp"
// actual data built from the data files, also contains infos that are global to them, like color icons

struct Data {
    json data; // the actual data, I just keep it as json even internally
    std::map<std::string, unsigned int> themeToID; // map that given a theme returns its ID
    // TODO: make these pointers or something
    Data(const std::string &dataDir);
    ~Data() = default;

    // parses the file, calculates most used theme, returns json with all the data
    json parsefile(const std::string &dataDir, const std::string &name) const;

    // entry point for menu queries
    // only the main table's data has the color-icons
    std::string menuFirst(std::string &input);

    // debug print
    void print() const;

    // entry point for read queries
    std::string read(std::string &input) const;

    // called when parsing menu queries, depending on the type of the data
    /*
    theme: theme that was clicked in the main menu
    input: query string parsed so far
    back: string to place as a path to go back
    name: name of option clicked to get here. need this to add into 'back' for the next recursive call
    data: json of current object
    color_icons: color icons
    */
    // std::string menu(const std::string &theme, std::string &input, const std::string &back, const std::string &name, json &data, const json &color_icons);
    std::string menuTable(const std::string &theme, std::string &input, const std::string &back, const std::string &name, json &_data, const json &color_icons);
    std::string menuList(const std::string &theme, std::string &input, const std::string &back, const std::string &name, json &_data, const json &color_icons);
    std::string menuListPicture(const std::string &theme, std::string &input, const std::string &back, const std::string &name, json &_data);

    // apply themes recursivelly to a table
    void applyAll(const std::string &theme, json &data);

    // get ID of a theme, given its name
    unsigned int getThemeID(const std::string &theme) const;
    // tries to find the element in the given data 
    std::string readElement(const std::string &name, std::string &input, const json &data) const;
    // already receives element itself
    std::string processElement(std::string &input, const json &element) const;

    // used by read queries, depending on the type of the data
    std::string readTable(std::string &input, const json &data) const;
    std::string readList(std::string &input, const json &data) const;
    std::string readListApply(std::string &input, const json &data) const;
    std::string readApply(std::string &input, const json &data) const;

    // dump
    void saveTo(const std::string &dataDir);
};

#endif
