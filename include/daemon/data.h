#ifndef DATA_H
#define DATA_H

#include "files.h"
// actual data built from the data files, also contains infos that are global to them, like color icons

struct Data {
	json data;
	std::map<std::string, unsigned int> themeToID;

	Data(const std::string &dataDir);
	~Data() = default;

	// parses the file, calculates most used theme, returns json with all the data
	json parsefile(const std::string &dataDir, const std::string &name) const;

	// only the main Data has the color-icons
	// instead of making another struct or something,
	// main should call this to get the color icons
	std::string menuFirst(std::string &input);

	void print() const;
	std::string read(std::string &input) const;
	std::string menu(std::string &input, const json &color_icons);

	unsigned int getThemeID(const std::string &theme) const;
	// tries to find the element in the given data 
	std::string readElement(const std::string &name, std::string &input, const json &data) const;
	// already receives element itself
	std::string processElement(std::string &input, const json &element) const;
	std::string readTable(std::string &input, const json &data) const;
	std::string readList(std::string &input, const json &data) const;
	std::string readListApply(std::string &input, const json &data) const;
	std::string readApply(std::string &input, const json &data) const;

	std::string menuItem(std::string &input, const json &color_icons, json &data);
};

#endif
