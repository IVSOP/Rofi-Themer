#include "data.h"
#include "errors.h"
#include "common.h"

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

std::string Data::menuItem(std::string &input, const json &color_icons, json &data) {

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

std::string Data::menu(std::string &input, const json &color_icons) {

}

std::string Data::menuFirst(std::string &input) {
	size_t pos = input.find('/');
	if (pos == std::string::npos) { // show main menu
		std::string res = "";
		for (auto& [theme, icon] : data["color-icons"].items()) {
			res += rofi_message("Theme " + theme, theme, icon);
		}

		return res + rofi_active(this->themeToID[this->data["theme"]]);
	} else {
		// std::string theme = input.substr(0, pos);
		// std::string options = input.substr(pos + 1); // from pos + 1 to the end
		// std::string back = "";
		// return this->main_table.menu(theme, options, themestr, back, this->color_icons);
		return "dsakjdsalkda";
	}
}
