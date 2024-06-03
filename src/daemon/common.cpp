#include "common.h"

#define INIT_ESCAPE '\0'
#define ESCAPE_SEP '\x1f'

std::string rofi_active(int active) {
	return std::string("") + INIT_ESCAPE + "active" + ESCAPE_SEP + std::to_string(active);
}

std::string rofi_active(const std::vector<int> &vec) {
	if (vec.size() == 0) return "";

	std::string res = std::string("") + INIT_ESCAPE + "active" + ESCAPE_SEP;
	unsigned int i;
	for (i = 0; i < vec.size() - 1; i++) {
		res += std::to_string(vec[i]) + ',';
	}
	res += std::to_string(vec[i]);
	return res;
}

std::string rofi_message(const std::string &text, const std::string &info) {
	return 
		text + INIT_ESCAPE +
		"info" + ESCAPE_SEP + info + "\n";
}

std::string rofi_message(const std::string &text, const std::string &info, const std::string &icon) {
	return 
		text + INIT_ESCAPE +
		"info" + ESCAPE_SEP + info + ESCAPE_SEP +
		"icon" + ESCAPE_SEP + icon + "\n";
}

std::string print_back(const std::string &info) {
	const std::string str = "Back";
	return rofi_message(str, info);
}

std::string print_all(const std::string &info) {
	const std::string str = "All";
	return rofi_message(str, info + "/*");
}
