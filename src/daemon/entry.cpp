#include "entry.h"
#include "table.h"
#include "errors.h"

Entry::Entry(const std::string &name, std::string &line, const std::string &dataDir, int numThemes) {
	// first is the type, this determines what parsing will be done
	size_t pos = line.find(';');
	std::string typestr = line.substr(0, pos);
	std::string data = line.substr(pos + 1);

#ifdef DEBUG
	printf("parsing entry %s. Type: %s\n", name.c_str(), typestr.c_str());
#endif
	// second is what theme was selected

	// very inefficient but it will be fast anyway, idc
	if (typestr == "apply") {
		this->type = APPLY;
		parseApply(data);
	} else if (typestr == "apply_list") {
		this->type = APPLY_LIST;
		parseApplyList(data);
	} else if (typestr == "sub") {
		this->type = SUB;
		parseSub(name, dataDir, numThemes);
	} else if (typestr == "list") {
		this->type = LIST;
		parseList(data, false);
	} else if (typestr == "list_picture") {
		this->type = LIST_PICTURE;
		parseList(data, true);
	} else {
		print_error("Invalid type");
		fprintf(stderr, "'%s'\n", typestr.c_str());
		exit(1);
	}
}

void Entry::parseApply(std::string &line) {
	// just add every entry to vector of strings
	std::vector<std::string> vec = std::vector<std::string>();

	size_t pos = 0;
	std::string token;

	// first token is a number
	pos = line.find(';');
	// if pos == std::string::npos.....
	this->active_theme = std::stoi(line);
	line.erase(0, pos + 1);

	// loops like these are do while since the last element is ";..." and not ";...;"

	do {
		pos = line.find(';');
		token = line.substr(0, pos);
		if (token.length() > 0) {
			vec.push_back(token);
		} else {
			vec.emplace_back(""); // for simplicity, write empty string when no value is found
		}
		line.erase(0, pos + 1); // 1 is len of delimiter
		// puts(token.c_str());
	} while (pos != std::string::npos);

	this->data = vec;
}

void Entry::parseSub(const std::string &name, const std::string dataDir, int numThemes) {
	this->data.emplace<SUB_DATA>(std::make_unique<Table>(dataDir + name + ".tb", dataDir, numThemes));
	this->active_theme = std::get<SUB_DATA>(this->data).get()->calcMostUsed(numThemes);
}

void Entry::parseList(std::string &line, bool show_pictures) {
	// got lazy moving this to main function
	size_t pos = line.find(';');
	// if pos == std::string::npos.....
	this->active_theme = std::stoi(line);
	line.erase(0, pos + 1);

	this->data.emplace<LIST_DATA>(std::make_unique<List>(line, show_pictures));
}

// bad at naming things, parses [....] into vec of strings
void parseApplyListAux(std::string &list, std::vector<std::string> &vec) {
	size_t pos;
	std::string token;
	do {
		pos = list.find(';');
		token = list.substr(0, pos);
		if (token.length() > 0) {
			vec.push_back(token);
		} else {
			vec.emplace_back(""); // for simplicity, write empty string when no value is found
		}
		list.erase(0, pos + 1); // 1 is len of delimiter
		// puts(token.c_str());
	} while (pos != std::string::npos);
}

void Entry::parseApplyList(std::string &line) {
	// got lazy moving this to main function
	size_t pos = line.find(';');
	// // if pos == std::string::npos.....
	this->active_theme = std::stoi(line);
	line.erase(0, pos + 1);

	std::vector<std::vector<std::string>> main_vec;
	

	// separate all [lists], parse them individually
	size_t start = line.find('['),
	end = line.find(']');
	std::string token;
	while (start != std::string::npos) {
		// std::cout << "parsing line " << line << std::endl;
		token = line.substr(start + 1, end - 1 - start);
		// std::cout << "parsing list " << token << std::endl;
		std::vector<std::string> vec;
		parseApplyListAux(token, vec);
		main_vec.push_back(vec);

		line.erase(0, end + 1);
		start = line.find('['),
		end = line.find(']');
	}

	this->data = main_vec;
}

void Entry::print(int depth_level) const {
	switch (this->type) {
		case APPLY:
			for (const std::string &str : std::get<APPLY_DATA>(this->data)) {
				std::cout << str << ";";
			}
			std::cout << std::endl;
			break;

		case APPLY_LIST: // why is the depth_level different here from everywhere else wtf
			std::cout << std::endl;
			std::cout << std::setw(depth_level * 4) << "" << "{" << std::endl;
			for (const auto &vec : std::get<APPLY_LIST_DATA>(this->data)) {
				for (const auto &str : vec) {
					std::cout << std::setw((depth_level + 1)  * 4) << "" << str << ";";
				}
				std::cout << std::endl;
			}
			std::cout << std::setw(depth_level * 4) << "" << "} (apply)" << std::endl;
			break;

		case SUB:
			std::cout << std::endl;
			std::get<SUB_DATA>(this->data).get()->print(depth_level + 1);
			break;

		case LIST:
			std::cout << std::endl;
			std::get<LIST_DATA>(this->data).get()->print(depth_level + 1);
			break;

		case LIST_PICTURE:
			std::cout << std::endl;
			std::get<LIST_DATA>(this->data).get()->print(depth_level + 1);
			break;
	}
}

std::string Entry::read(std::string &input) const {
	switch (this->type) { // not making a jump table due to poor readability, I will assume compiler does it for me
		case APPLY:
			{ // wtf
				// check if 0 size????
				const APPLY_DATA &vec = std::get<APPLY_DATA>(this->data);
				return vec[this->active_theme] + "\n";
				break;
			}

		case APPLY_LIST:
			{
				const APPLY_LIST_DATA &vec = std::get<APPLY_LIST_DATA>(this->data);
				std::string res = "";
				for (const std::string &str : vec[this->active_theme]) {
					res += str + "\n";
				}
				return res;
			}

		case SUB:
			return std::get<SUB_DATA>(this->data).get()->read(input);
			break;

		case LIST:
			return std::get<LIST_DATA>(this->data).get()->read(this->active_theme);
			break;

		case LIST_PICTURE:
			return std::get<LIST_DATA>(this->data).get()->read(this->active_theme);
			break;

		default: // I will assume the type can never be anything else ever, but this way compiler shuts up
			return "error";
	}
}

std::string Entry::print_option(const std::string &name, const std::string &info, const std::vector<std::string> &color_icons) const {
	return rofi_message(name, color_icons[this->active_theme], info);
}

// print only func, called only by the table. otherwise, apply and apply list would have teir options changed
std::string Entry::menu(const std::string &name, const std::string &info, const std::vector<std::string> &color_icons) const {
	return print_option(name, info + "/" + name, color_icons);
}

void Entry::applyAll(int theme, int numThemes) {
	switch (this->type) { // not making a jump table due to poor readability, I will assume compiler does it for me
		case APPLY:
			if (applyHasDataFor(theme)) {
				this->active_theme = theme;
			}
			break;

		case APPLY_LIST:
			if (applyListHasDataFor(theme)) {
				this->active_theme = theme;
			}
			break;

		case SUB:
			std::get<SUB_DATA>(this->data).get()->applyAll(theme, numThemes);
			this->active_theme = std::get<SUB_DATA>(this->data).get()->calcMostUsed(numThemes);
			break;

		case LIST:
			{		
				List *list = std::get<LIST_DATA>(this->data).get();
				if (list->hasDataFor(theme)) {
					this->active_theme = theme;
					list->applyAll();
				}
				break;
			}

		case LIST_PICTURE:
			{		
				List *list = std::get<LIST_DATA>(this->data).get();
				if (list->hasDataFor(theme)) {
					this->active_theme = theme;
					list->applyAll();
				}
				break;
			}
	}
}

bool Entry::applyHasDataFor(int theme) const {
	const APPLY_DATA &vec = std::get<APPLY_DATA>(this->data);
	if (vec[theme].size() == 0) return false;

	return true;
}

bool Entry::applyListHasDataFor(int theme) const {
	const APPLY_LIST_DATA &vec = std::get<APPLY_LIST_DATA>(this->data);
	if (vec[theme].size() == 0) return false;

	return true;
}
