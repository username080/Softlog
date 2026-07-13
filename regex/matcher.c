#include "matcher.h"
#include <stddef.h>

bool check_log_line(const char* line, regex_t* compiled_regex) {
    if (!line || !compiled_regex) {
        return false;
    }
    
    // regexec returns 0 for a successful match.
    // We pass REG_NOSUB to regcomp, so we don't need to pass a matches array here.
    int ret = regexec(compiled_regex, line, 0, NULL, 0);
    return (ret == 0);
}
