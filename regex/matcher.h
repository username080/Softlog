#ifndef MATCHER_H
#define MATCHER_H

#include <stdbool.h>
#include <regex.h>

// Checks if the given log line matches the compiled regular expression.
bool check_log_line(const char* line, regex_t* compiled_regex);

#endif // MATCHER_H
