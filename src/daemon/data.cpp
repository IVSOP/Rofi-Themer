#include "data.h"
#include "errors.h"
#include "common.h"

#define ALL_STRING "*" // string representing ALL. whatever you put here will be a reserved name, no element in your json can have this name

Data::Data(const std::string &dataDir)
: data(parsefile(dataDir, "main.json"))
{

// #ifdef DEBUG
// 	printf("parsing table from %s\n", path.c_str());
// #endif

	unsigned int i = 0;
	for (auto& [key, _] : data["color-icons"].items()) {
		this->themeToID[key] = i;
		i++;
	}
}

json Data::parsefile(const std::string &dataDir, const std::string &name) const {
	std::cout << "Parsing " << dataDir + name << std::endl;

	FileHandler filehandler(dataDir + name);

	json parsed = filehandler.readjson();

	std::unordered_map<std::string, unsigned int> themes;


	for (auto& [key, element] : parsed["data"].items()) {
		// if (! key.starts_with('_')) { // elements starting with _ are not considered. other option was data: {options here} but then the code would be a mess
		if (element["type"] == "table") {
			// just to avoid deleting it early
			// I trust the compiler will figure this out
			json newElement = parsefile(dataDir, key + ".json");
			newElement["type"] = "table";
			element = newElement;
		}

		// store the themes of the children
		auto theme = themes.find(element["theme"]);
		if (theme != themes.end()) {
			theme->second ++;
		} else {
			themes[element["theme"]] = 0;
		}
		// }
	}

	// calculate the most used theme
	unsigned int max = 0;
	std::string theme = "";
	for (const std::pair<const std::string, unsigned int>& pair : themes) {
		// If the current value is greater than the current maximum value
		if (pair.second > max) {
			// Update the maximum value and corresponding string
			max = pair.second;
			theme = pair.first;
		}
	}
    
	parsed["theme"] = theme;

	return parsed;
}

void Data::print() const {
	std::cout << data.dump(4) << std::endl;
}

unsigned int Data::getThemeID(const std::string &theme) const {
	// error checking????
	auto res = themeToID.find(theme);
	if (res != themeToID.end()) {
		return res->second;
	} else {
		// error checking??????????????
		return static_cast<unsigned int>(-69); // overflow, guarantee a wrong result
	}
}

std::string Data::readTable(std::string &input, const json &_data) const {
	const json &data = _data["data"];
	size_t pos = input.find('/');
	if (pos == std::string::npos) {
		const std::string name = input;

		if (name == "*") {
			std::string res = "";
			for (const auto& [key, element] : data.items()) {
				// printf("processElement is processing %s\n", key.c_str());
				res += processElement(input, element);
				res += '\n';
			}

			res.pop_back(); // remove extra \n
			return res;
		} else {
			input = "";
			return readElement(name, input, data);
		}

	} else {
		const std::string name = input.substr(0, pos);
		input.erase(0, pos + 1);
		return readElement(name, input, data);
	}

	return "Error in reading table: input is " + input;
}

std::string Data::readList(std::string &input, const json &data) const {
	size_t pos = input.find('/');
	if (pos != std::string::npos) {
		return "Error: invalid input for list";
	}

	// [options][theme][selected]
	return data["options"][getThemeID(data["theme"])][data["selected"].get<unsigned int>()];
}

std::string Data::readListApply(std::string &input, const json &data) const {
	size_t pos = input.find('/');
	if (pos != std::string::npos) {
		return "Error: invalid input for list";
	}

	// [options][theme], result is a list
	std::string res = "";
	for (const std::string str : data["options"][getThemeID(data["theme"])]) {
		res += str;
		res += '\n';
	}

	// delete last \n
	res.pop_back();

	return res;
}

std::string Data::readApply(std::string &input, const json &data) const {
	size_t pos = input.find('/');
	if (pos != std::string::npos) {
		return "Error: invalid input for apply";
	}

	// [options][theme]
	return data["options"][getThemeID(data["theme"])];
}


// all this does is redirect to the correct read function
std::string Data::readElement(const std::string &name, std::string &input, const json &_data) const {

	// optimize with a jump table or something....................
	// call processElement??
	// printf("Read element is looking for name %s\n", name.c_str());

	// std::cout << _data.dump(2) << std::endl;
	const json &data = _data[name];
	const std::string &type = data["type"];

	if (type == "table") {
		return readTable(input, data);
	} else if (type == "apply") {
		return readApply(input, data);
	} else if (type == "list" || type == "list_picture") {
		return readList(input, data);
	} else if (type == "apply_list") {
		return readListApply(input, data);
	}

	return "Error: type " + type + " is invalid";
}

std::string Data::processElement(std::string &input, const json &element) const {
	const std::string &type = element["type"];

	if (type == "table") {
		return readTable(input, element);
	} else if (type == "apply") {
		return readApply(input, element);
	} else if (type == "list" || type == "list_picture") {
		return readList(input, element);
	} else if (type == "apply_list") {
		return readListApply(input, element);
	}

	return "Error: type " + type + " is invalid";
}

std::string Data::read(std::string &input) const {
	return readTable(input, data);
}

std::string calcMostUsed(json &data) {
	const std::string &type = data["type"];

	std::string mostused = "";

	if (type == "table") {
		// need to calculate average from all children
		std::unordered_map<std::string, unsigned int> themes;
		std::string element_theme;
		for (auto& [key, element] : data["data"].items()) {
			element_theme = calcMostUsed(element);
			auto theme = themes.find(element_theme);
			if (theme != themes.end()) {
				theme->second ++;
			} else {
				themes[element_theme] = 0;
			}
		}

		unsigned int max = 0;
		std::string theme = "";
		for (const std::pair<const std::string, unsigned int>& pair : themes) {
			// If the current value is greater than the current maximum value
			if (pair.second > max) {
				// Update the maximum value and corresponding string
				max = pair.second;
				theme = pair.first;
			}
		}
		mostused = theme;
		data["theme"] = mostused;
	} else if (type == "apply") {
		mostused = data["theme"];
	} else if (type == "list" || type == "list_picture") {
		mostused = data["theme"];
	} else if (type == "apply_list") {
		mostused = data["theme"];
	}

	return mostused;
}

std::string Data::menuList(const std::string &theme, std::string &input, const std::string &back, const std::string &name, json &data, const json &color_icons) {
	size_t pos = input.find('/');
	if (pos != std::string::npos) {
		int option = std::stoi(input.substr(0, pos));
		data["selected"] = option;
		// theme may or may not be the same here
		data["theme"] = theme;
	}

	int themeID = getThemeID(theme);
	int selected = data["selected"];

	// display list options is always done no matter what
	std::string res = "";
	int i = 0;
	for (auto& element : data["options"][themeID]) {
		if (i == selected) {
			// explicit otherwise gcc 14 shits itself
			std::string theme = data["theme"];
			std::string color_icon = color_icons[theme];
			res += rofi_message(element, back + name + "/" + std::to_string(i) + "/", color_icon);
		} else {
			res += rofi_message(element, back + name + "/" + std::to_string(i) + "/");
		}
		i++;
	}


	res += print_back(back);
	if (data["theme"] == theme) {
		res += rofi_active(selected);
	}

	return res;
}

std::string Data::menuListPicture(const std::string &theme, std::string &input, const std::string &back, const std::string &name, json &data) {
	size_t pos = input.find('/');
	if (pos != std::string::npos) {
		int option = std::stoi(input.substr(0, pos));
		data["selected"] = option;
		// theme may or may not be the same here
		data["theme"] = theme;
	}

	int themeID = getThemeID(theme);
	int selected = data["selected"];

	// display list options is always done no matter what
	std::string res = "";
	int i = 0;
	for (auto& element : data["options"][themeID]) {
		res += rofi_message(element, back + name + "/" + std::to_string(i) + "/", element);
		i++;
	}
	return res + print_back(back) + rofi_active(selected);
}

std::string Data::menuTable(const std::string &theme, std::string &input, const std::string &back, const std::string &name, json &_data, const json &color_icons) {
	json &data = _data["data"];
	size_t pos = input.find('/');
	std::string res = rofi_message("All", back + name + "/" + ALL_STRING + "/", color_icons[theme]);
	if (pos == std::string::npos) { // display all entries in the table
		std::vector<int> ids;
		int i = 1;
		std::string theme, color_icon;
		for (auto& [key, element] : data.items()) {
			theme = element["theme"];
			color_icon = color_icons[theme];
			res += rofi_message(key, back + name + "/" + key + "/", color_icon);
			// printf("info: %s\n", (back + name + "/" + key + "/").c_str());
			if (element["theme"] == theme) {
				ids.push_back(i);
			}
			i++;
		}
		// had to return here since active needs to come last
		return res + print_back(back) + rofi_active(ids);
	} else { // go into table option
		const std::string nextname = input.substr(0, pos);

		if (nextname == ALL_STRING) {
			applyAll(theme, _data);
			input = "";
			return menuTable(theme, input, back, name, _data, color_icons);
		}

		// printf("table option selected: %s input is %s\n", nextname.c_str(), input.c_str());
		input.erase(0, pos + 1);

		json &next = data[nextname];
		const std::string &type = next["type"];

		if (type == "table") { // go one level deeper
			return menuTable(theme, input, back + name + "/", nextname, next, color_icons);
		} else if (type == "apply" || type == "apply_list") { // apply the option, stay in the same menu
			next["theme"] = theme;
			return menuTable(theme, input, back, name, _data, color_icons); // parse this data again, but this time input has been consumed
		} else if (type == "list") { // go into the list
			return menuList(theme, input, back + name + "/", nextname, next, color_icons);
		} else if (type == "list_picture") { // go into the picture list
			return menuListPicture(theme, input, back + name + "/", nextname, next);
		}

		res = "Error";
	}

	return res + print_back(back);
}

void Data::applyAll(const std::string &theme, json &data) {
	const std::string &type = data["type"];

	if (type == "table") {
		for (auto& element : data["data"]) {
			applyAll(theme, element);
		}
	} else if (type == "list" || type == "list_picture") { // if theme changed, set to option 0
		if (data["theme"] != theme) {
			data["selected"] = 0;
		}
	}

	data["theme"] = theme;
}


std::string Data::menuFirst(std::string &input) {
	size_t pos = input.find('/');
	if (pos == std::string::npos) { // show main menu
		std::string res = "";
		for (auto& [theme, icon] : this->data["color-icons"].items()) {
			res += rofi_message("Theme " + theme, theme + "/", icon);
		}

		return res + rofi_active(getThemeID(this->data["theme"]));
	} else {
		std::string theme = input.substr(0, pos);
		input.erase(0, pos + 1);
		std::string back = "";

		// printf("goint into a table. theme is %s. input is %s\n", theme.c_str(), input.c_str());
		
		std::string res = menuTable(theme, input, back, theme, this->data, this->data["color-icons"]);
		this->data["theme"] = calcMostUsed(this->data);
		return res;
	}
}
