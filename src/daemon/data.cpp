#include "data.h"
#include <fstream>
#include "errors.h"

Data::Data(const std::string &dir)
: color_icons(), main_table(dir + "main.tb")
{
	parseColors(dir + "color-icons");
}

void Data::parseColors(const std::string &path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		print_error("Error opening file");
		exit(1);
    }

	std::string line;
	const std::string delim = "#";
    while (std::getline(file, line)) {
		// ignore comments, only valid at the start of the line for now
		// if (line.compare(0, delim.length(), delim) != 0) {
        	this->color_icons.emplace_back(line);
		// } did not implement this, when files get overwritten it all goes away
    }

	file.close();
}

void Data::print() {
	puts("Color icons are:");
	for (const std::string &str : color_icons) {
		puts(str.c_str());
	}

	puts("Data:");
	main_table.print(1);
}

std::string Data::read(std::string &input) const {
	size_t pos = input.find('/');
	input.erase(0, pos + 1);

	return this->main_table.read(input);
}

std::string Data::menu(std::string &input) {
	size_t pos = input.find('/');
	if (pos == std::string::npos) { // show main menu
		std::vector<int> themes = this->main_table.getThemes(this->color_icons.size());
		std::string res = "";
		// display main menu. done here and not in the table itself since it is an exception that can only happen at the start
		for (unsigned int i = 0; i < this->color_icons.size(); i++) {
			res += rofi_message("Theme " + std::to_string(i) + "(" + std::to_string(themes[i]) + "/" + std::to_string(this->main_table.data.size()) + ")", 
				this->color_icons[i], std::to_string(i) + "/");
		}
		return res + rofi_active(this->main_table.calcMostUsed()); // no need for calcMostUsed(), but whatever. Table does not know its theme directly, but there is no Entry to wrap it here
		// TODO got lazy, could use themes vector to calculate this here
	} else {
		int theme = std::stoi(input.substr(0, pos));
		std::string options = input.substr(pos + 1); // from pos + 1 to the end
		std::string themestr = std::to_string(theme);
		std::string back = "";
		return this->main_table.menu(theme, options, themestr, back, this->color_icons);
	}
}
