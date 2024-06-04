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


	unsigned int getThemeID(const std::string &theme) const;
	// tries to find the element in the given data 
	std::string readElement(const std::string &name, std::string &input, const json &data) const;
	// already receives element itself
	std::string processElement(std::string &input, const json &element) const;
	std::string readTable(std::string &input, const json &data) const;
	std::string readList(std::string &input, const json &data) const;
	std::string readListApply(std::string &input, const json &data) const;
	std::string readApply(std::string &input, const json &data) const;
};

#endif
