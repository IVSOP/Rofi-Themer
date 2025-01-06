#include "crash.hpp"
#include "logs.hpp"

void crash(const std::string &title, const std::string &text) {
	Log::log(LOG_TYPE::ERR, title, text);
	exit(EXIT_FAILURE);
}

