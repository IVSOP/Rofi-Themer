#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>
#include <iomanip>
#include <iostream>
#include <cstdint>

// type of entry
// ended up not using this powers of 2, didnt know how to do it in a switch statement
enum TYPE {
	APPLY = 1,        // apply this setting
	SUB = 2,          // goes into subtable
	LIST = 4,         // show list of options, can only pick 1
	LIST_PICTURE = 8, // same but strings are pictures
	APPLY_LIST = 16
};

#define APPLY_DATA std::vector<std::string>
#define APPLY_LIST_DATA std::vector<std::vector<std::string>> // not a List because easier this way, no other reason, might change
#define SUB_DATA std::unique_ptr<Table> // case of subtable is represented as a new table. pointer due to circular dependency
#define LIST_DATA std::unique_ptr<List> // case of being a list, cannot be a table unfortunately. could be object and not pointer, but this makes it consistent

std::string rofi_active(int active);
std::string rofi_active(const std::vector<int> &vec);
std::string rofi_message(const std::string &text, const std::string &info); // no icon
std::string rofi_message(const std::string &text, const std::string &info, const std::string &icon);

std::string print_back(const std::string &info);
std::string print_all(const std::string &info);

#endif
