#include "crash_print.hpp"

void crash_print(const std::string &title, const std::string &text) {
	fprintf(stderr, "%s: %s\n", title.c_str(), text.c_str());
	exit(EXIT_FAILURE);
}

