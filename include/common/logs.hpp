#ifndef LOGS_HPP
#define LOGS_HPP

#include <string>
#include "files.hpp"

#define print_error(msg) Log::log(LOG_TYPE::ERR,\
								  std::string(__FILE__) + "," + std::string(__PRETTY_FUNCTION__) + "," + std::to_string(__LINE__), msg)

enum LOG_TYPE {
	INFO = 0,
	WARN = 1,
	ERR  = 2,
	UNIMPORTANT = 4
};

#define LOGFILE "/tmp/Rofi-Themer.log"

// kind of a fucked up singleton, I hate the slop of calling getInstance() and much prefer Log::func()
// // so the file handler instance is unique
struct Log {
	Log() = delete;

    static FileHandler logfile;

	static void log(const std::string &text);
	static void log(const std::string &title, const std::string &text);
	static void log(LOG_TYPE type, const std::string &text);
	static void log(LOG_TYPE type, const std::string &title, const std::string &text);
};

#endif

