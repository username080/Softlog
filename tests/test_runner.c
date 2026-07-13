#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "../regex/matcher.h"
#include "../utils/json.h"

int tests_run = 0;
int tests_passed = 0;

#define ASSERT(expr) do { \
    tests_run++; \
    if (!(expr)) { \
        fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, #expr); \
    } else { \
        tests_passed++; \
    } \
} while(0)

void test_regex_matcher() {
    printf("Running test_regex_matcher...\n");
    
    regex_t regex;
    regcomp(&regex, "^FATAL:.*error [0-9]+$", REG_EXTENDED | REG_NOSUB);

    // Should match
    ASSERT(check_log_line("FATAL: db connection error 404", &regex) == true);
    
    // Should NOT match
    ASSERT(check_log_line("WARNING: error 502", &regex) == false);
    ASSERT(check_log_line("FATAL: connection error foo", &regex) == false);
    
    // Null checks (matcher.c should safely return false)
    ASSERT(check_log_line(NULL, &regex) == false);
    ASSERT(check_log_line("FATAL", NULL) == false);

    regfree(&regex);
}

void test_json_escaping() {
    printf("Running test_json_escaping...\n");
    
    char out_buf[1024];
    
    // Basic text
    ASSERT(escape_json_string("hello", out_buf, sizeof(out_buf)) == true);
    ASSERT(strcmp(out_buf, "hello") == 0);
    
    // Escape quotes
    ASSERT(escape_json_string("hello \"world\"", out_buf, sizeof(out_buf)) == true);
    ASSERT(strcmp(out_buf, "hello \\\"world\\\"") == 0);
    
    // Escape backslashes and newlines
    ASSERT(escape_json_string("C:\\path\n", out_buf, sizeof(out_buf)) == true);
    ASSERT(strcmp(out_buf, "C:\\\\path\\n") == 0);
    
    // Buffer overflow protection test
    char small_buf[5];
    ASSERT(escape_json_string("12345", small_buf, sizeof(small_buf)) == false);
}

int main() {
    printf("Starting Unit Tests...\n");
    
    test_regex_matcher();
    test_json_escaping();
    
    printf("\nTests Run: %d\n", tests_run);
    printf("Tests Passed: %d\n", tests_passed);
    
    if (tests_run == tests_passed) {
        printf("SUCCESS! All tests passed.\n");
        return 0;
    } else {
        printf("FAILURE! %d tests failed.\n", tests_run - tests_passed);
        return 1;
    }
}
