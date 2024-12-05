#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <map>
#include <string>
#include "entry.h"

// data structure representing a table retrieved from data file
struct Table {
	std::map<std::string, Entry> data; // the actual data in the table, looked up by string

	Table() = default;
	Table(const std::string &path, const std::string &dataDir, int numThemes); // paths are all absolute, dataDir is used to parse subtables (was a bandaid fix, will work fine)
	~Table() = default;

	void print(int depth_level);

	int calcMostUsed(int numThemes) const;
	std::vector<int> getThemes(int numThemes) const;

	void applyAll(int theme, int numThemes);
	std::string getActive(int theme);
	std::string menu_all(int theme, const std::string &info, const std::string &back_info, const std::vector<std::string> &color_icons);

	std::string read(std::string &input) const;
	std::string showEntry(Entry &entry, const std::string &name, int theme, std::string &input, std::string &info, const std::string &back_info, const std::vector<std::string> &color_icons);
	std::string menu(int theme, std::string &input, std::string &info, const std::string &back_info, const std::vector<std::string> &color_icons);
};

#endif
