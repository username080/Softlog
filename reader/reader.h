#ifndef READER_H
#define READER_H

#include <regex.h>

// Starts tailing the log file using inotify purely on the main thread.
void start_tailing(const char* filepath, regex_t* compiled_regex, const char* original_pattern_str, const char* webhook_url);

#endif // READER_H
