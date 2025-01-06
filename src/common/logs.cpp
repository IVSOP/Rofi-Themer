#include "logs.hpp"

#include <cstdio>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"
#define GRAY "\033[1;30m"

void print_color(LOG_TYPE type) {
	switch(type) {
		case (LOG_TYPE::INFO):
			break;
		case (LOG_TYPE::WARN):
			printf("%s", YELLOW);
			break;
		case (LOG_TYPE::ERR):
			printf("%s", RED);
			break;
		case (LOG_TYPE::UNIMPORTANT):
			printf("%s", GRAY);
			break;
	}
}

void reset_color() {
	printf("%s", RESET);
}

void print(LOG_TYPE type, const std::string &title, const std::string &text) {
    Log::logfile.file << "title: " << title << " text: " << text << std::endl;
	// if (title.length() > 0) {
	// 	print_color(type);
	// 	printf("=== %s ===\n", title.c_str());
	// 	reset_color();
	// 	printf("%s\n", text.c_str());
	// } else {
	// 	printf("%s\n", text.c_str());
	// }
}

FileHandler Log::logfile = FileHandler(LOGFILE, FileModes::Trunc);

void Log::log(const std::string &text) {
	print(LOG_TYPE::INFO, std::string(), text);
}

void Log::log(const std::string &title, const std::string &text) {
	print(LOG_TYPE::INFO, title, text);
}

void Log::log(LOG_TYPE type, const std::string &text) {
	print(type, std::string(), text);
}

void Log::log(LOG_TYPE type, const std::string &title, const std::string &text) {
	print(type, title, text);
}

