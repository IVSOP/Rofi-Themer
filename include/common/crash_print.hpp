#ifndef CRASH_PRINT_H
#define CRASH_PRINT_H

#include "stdlib.hpp"

void crash_print(const std::string &title, const std::string &text);
// use this macro, this way when releasing it can be made to do nothing
// TODO I'm pretty sure that s not how it works, check if condition is evaluated or not
#define CRASH_IF(condition, text) if (condition) crash_print(std::string(__PRETTY_FUNCTION__), text); // use __func__ instead??
#define CRASH_IF_TITLE(condition, title, text) if (condition) crash_print(title, text);

#endif

