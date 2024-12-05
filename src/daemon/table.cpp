#include "table.h"
#include <fstream>
#include "errors.h"
#include <algorithm>
#include "files.h"

Table::Table(const std::string &path, const std::string &dataDir, int numThemes) {
	FileHandler filehandler(path);

#ifdef DEBUG
	printf("parsing table from %s\n", path.c_str());
#endif

	std::string line = "";
	size_t pos;
    while (filehandler.getline(line)) {
		pos = line.find(';');
		std::string name = line.substr(0, pos);
		std::string data = line.substr(pos + 1);

		// WHAT THE FUCK?????? most cursed thing I have ever seen
		this->data.emplace(
			::std::piecewise_construct, // special to enable forwarding
			::std::forward_as_tuple(name), // arguments for key constructor
			::std::forward_as_tuple(name, data, dataDir, numThemes) // arguments for value constructor
		);
		// this->data.emplace(name, Entry(name, data)); // error wtf???
    }
}

void Table::print(int depth_level) {
	std::cout << std::setw((depth_level - 1) * 4) << "" << "[" << std::endl;
	for (const auto &entrypair : this->data) {
		std::cout << std::setw(depth_level * 4) << "" << entrypair.first << " (" << entrypair.second.active_theme << "): ";
		entrypair.second.print(depth_level);
	}
	std::cout << std::setw((depth_level - 1) * 4) << "" << "]" /* most used: " << this->most_used */ << std::endl;
}

std::string Table::read(std::string &input) const {
	size_t pos = input.find('/');
	// if (pos == std::string::npos) {
	// 	print_error("read is invalid");
	// }
	std::string name = input.substr(0, pos);
	if (name == "*") { // get all the data
		std::string wildcard = "*"; // got lazy
		std::string res = "";
		for (const auto &entrypair : this->data) { // got lazy iterating values only
			res += entrypair.second.read(wildcard);
		}
		return res;
	} else {
		try {
			const Entry &entry = this->data.at(name);
						
			input.erase(0, pos + 1); // 1 is len of delimiter
			return entry.read(input);

		} catch (const std::out_of_range& e) {
			printf("'%s' does not exist in the map\n", name.c_str());
			exit(EXIT_FAILURE);
		}
	}
}

// calculate most used themes
int Table::calcMostUsed(int numThemes) const {
	std::vector<int> themes(numThemes);

	for (unsigned int i = 0; i < themes.size(); i++) {
		themes[i] = 0;
	}
	for (const auto &entrypair : this->data) {
		themes[entrypair.second.active_theme] += 1; // if not set, will be -1, segfault prob
	}
	
	// this is to find the max, got lazy
	return std::distance(themes.begin(), std::max_element(themes.begin(), themes.end()));
}

std::vector<int> Table::getThemes(int numThemes) const {
	std::vector<int> themes(numThemes);

	for (unsigned int i = 0; i < themes.size(); i++) {
		themes[i] = 0;
	}
	for (const auto &entrypair : this->data) {
		themes[entrypair.second.active_theme] += 1; // if not set, will be -1, segfault prob
	}

	return themes;
}

// info + "/" + name repeated seems bad, but makes easier to implement 'Back'
// very unfortunate that it had to be done this way, for example applying an option needs to print the parent table, it would be a mess if this was controled by entries themselves and not parent table
// the readonly function is still there though
std::string Table::showEntry(Entry &entry, const std::string &name, int theme, std::string &input, std::string &info, const std::string &back_info, const std::vector<std::string> &color_icons) {
	switch (entry.type) {
		case APPLY: // no further parsing required, option needs to be set. maybe see if string is "" for safety?
			if (entry.applyHasDataFor(theme)) {
				entry.active_theme = theme;
			}
			return menu_all(theme, info, back_info, color_icons);
			break;
		case APPLY_LIST:
			if (entry.applyListHasDataFor(theme)) {
				entry.active_theme = theme;
			}
			return menu_all(theme, info, back_info, color_icons);
			break;
		case SUB: // need to go into subtable
			{
				std::string why_is_this_needed = info + "/" + name;
				std::string back = info + "/"; // 😭😭 I'm in way too deep to change this now, in the future might redo the whole info thing
				std::string res = std::get<SUB_DATA>(entry.data).get()->menu(theme, input, why_is_this_needed, back, color_icons);
				
				// this is a very bad hack, have to change. Without it, going into subtable and changing things will update the theme of the table itself but not in the entry it is inside of, since it is lost
				entry.active_theme = std::get<SUB_DATA>(entry.data).get()->calcMostUsed(color_icons.size());

				return res;
				break;
			}
		case LIST:
			{
				std::string why_is_this_needed = info + "/" + name;
				std::string back = info + "/";
				std::string res = std::get<LIST_DATA>(entry.data).get()->menu(theme, &entry.active_theme, input, why_is_this_needed, back, color_icons);
				return res;
				break;
			}
		case LIST_PICTURE:
			{
				std::string why_is_this_needed = info + "/" + name;
				std::string back = info + "/";
				std::string res = std::get<LIST_DATA>(entry.data).get()->menu(theme, &entry.active_theme, input, why_is_this_needed, back, color_icons);
				return res;
				break;
			}
		default: // I will assume the type can never be anything else ever, but this way compiler shuts up
			return "error";
	}
}

// show all options in menu format
std::string Table::menu_all(int theme, const std::string &info, const std::string &back_info, const std::vector<std::string> &color_icons) {
	std::string res = print_all(info);
	for (const auto &entrypair : this->data) { // got lazy iterating values only
		// printf("Adding %s\n", entrypair.first.c_str());
		res += entrypair.second.menu(entrypair.first, info, color_icons); // this menu func only displays their name
	}
	// write(STDOUT_FILENO, res.c_str(), res.size());
	res += print_back(back_info) + getActive(theme);
	return res;
}

std::string Table::getActive(int theme) {
	// only display entry as active it it's theme is the same as 'theme'
	std::vector<int> vec;
	int i = 1; // to offset the 'All' option
	for (const auto &entrypair : this->data) { // got lazy iterating values only
		if (entrypair.second.active_theme == theme) {
			vec.push_back(i);
		}
		i++;
	}
	return rofi_active(vec);
}

void Table::applyAll(int theme, int numThemes) {
	for (auto &entrypair : this->data) { // got lazy iterating values only
		entrypair.second.applyAll(theme, numThemes);
	}
}

// info needs to be passed, so that all the tables before it can add things to it, otherwise it would be lost
// in the input string that were parsed were erased

// input is string that gets parsed
// info is accumulator of the path through the data structure to get here

// back_info is a hack to make it easier to get info for 'Back' (egg. info = a/b/c, back = a/b)
std::string Table::menu(int theme, std::string &input, std::string &info, const std::string &back_info, const std::vector<std::string> &color_icons) {
	// if there is no /, pos will be very big and the token will be the entire input
	
	size_t pos = input.find('/');
	std::string token = input.substr(0, pos);
	input.erase(0, pos); // 1 is len of delimiter
	// ugly hack for when no / is found, doing pos + 1 would overflow (at least I whink that's what happened when I tried it)
	input.erase(0, 1);
	printf("table received info: '%s'\n", info.c_str());


	if (token.length() == 0) { // show menu of this table
		return menu_all(theme, info, back_info, color_icons);
	} else if (token == "*") { // apply all options on this table
		applyAll(theme, color_icons.size());
		return menu_all(theme, info, back_info, color_icons);
	} else {
		try {
			Entry &entry = this->data.at(token);

			return showEntry(entry, token, theme, input, info, back_info, color_icons);
			// std::string rofi_str = entry.menu(token, theme, input, info, color_icons);

		} catch (const std::out_of_range& e) {
			printf("'%s' does not exist in the map\n", token.c_str());
			exit(EXIT_FAILURE);
		}
		
	}
}
