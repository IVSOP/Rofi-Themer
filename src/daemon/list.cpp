#include "list.h"

void List::parseList(std::string list) {
	// printf("got list %s\n\n", list.c_str());
	size_t pos;
	std::string token;
	std::vector<std::string> vec;

	do {
		pos = list.find(';');
		token = list.substr(0, pos);
		if (token.length() > 0) vec.push_back(token); // pretty sure this never happens, why would you have empty values in a list
		list.erase(0, pos + 1); // 1 is len of delimiter
		// puts(token.c_str());



	} while (pos != std::string::npos);

	this->data.push_back(vec); // WARNING THIS ASSUMES CORRECT THEME ORDER
}

List::List(std::string &line, bool show_pictures)
: data(), show_pictures(show_pictures)
{
	// get selected option, should be first number
	size_t start = line.find(';');
	this->selected_option = std::stoi(line);
	line.erase(0, start + 1);

	// separate all [lists], parse them individually
	start = line.find('[');
	line.erase(0, start + 1); // so that getting end is correct
	size_t end = line.find(']');
	std::string token;

	while (start != std::string::npos) {
		token = line.substr(0, end); // 0 since start was erased
		parseList(token);

		line.erase(0, end + 1);
		start = line.find('[');
		line.erase(0, start + 1); // so that getting end is correct
		end = line.find(']');
	}
}

void List::print(int depth_level) {
	std::cout << std::setw((depth_level - 1) * 4) << "" << "{" << std::endl;
	for (const auto &vec : this->data) {
		std::cout << std::setw(depth_level * 4) << "";
		for (const auto &str : vec) {
			std::cout << "" << str << ";";
		}
		std::cout << std::endl;
	}
	std::cout << std::setw((depth_level - 1) * 4) << "" << "} selected: " << this->selected_option << std::endl;
}

std::string List::read(int active_theme) const {
	const std::vector<std::string> &vec = this->data[active_theme];
	return vec[this->selected_option] + "\n";
}

std::string List::menuOptions(int theme, int current_theme, const std::string &info, const std::string &back_info, const std::vector<std::string> &color_icons) {
	std::string res = "";
	if (this->show_pictures == true) {
		// icons are the data itself
		int i = 0;
		for (const std::string &str : this->data[theme]) {
			const std::string num_info = info + "/" + std::to_string(i);
			res += rofi_message(str, str, num_info);
			i ++;
		}
	} else {
		int i = 0;
		for (const std::string &str : this->data[theme]) {
			const std::string num_info = info + "/" + std::to_string(i);
			res += rofi_message(str, color_icons[current_theme], num_info);
			i ++;
		}
	}
	res += print_back(back_info);
	if (theme == current_theme) {
		res += getActive();
	}
	return res;
}

std::string List::menu(int theme, int *current_theme, std::string &input, std::string &info, const std::string &back_info, const std::vector<std::string> &color_icons) {
	if (input.length() == 0) { // meh but it works
		return menuOptions(theme, *current_theme, info, back_info, color_icons);
	} else {
		// option chosen
		// if (hasDataFor(theme)) { // no need for this, since if the results don't exist they would not be shown anyway (so not clickable)

		this->selected_option = std::stoi(input);
		*current_theme = theme;
		// printf("Current theme is now %d\n", *current_theme);
		return menuOptions(theme, *current_theme, info, back_info, color_icons);
	}
}

// aplies first option by default, should only be called if option exists
void List::applyAll() {
	this->selected_option = 0;
}

std::string List::getActive() const {
	return rofi_active(this->selected_option);
}

bool List::hasDataFor(int theme) const {
	if (this->data[theme].size() == 0) return false;

	return true;
}
