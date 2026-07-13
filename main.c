#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <curl/curl.h>
#include "reader/reader.h"

void print_usage(const char* prog_name) {
    fprintf(stderr, "Usage: %s -f <log_file> -k <keyword_regex> -w <webhook_url>\n", prog_name);
    fprintf(stderr, "Example: %s -f /var/log/syslog -k \"^FATAL\" -w \"https://discord.com/api/webhooks/...\"\n", prog_name);
}

int main(int argc, char* argv[]) {
    const char* log_file = NULL;
    const char* keyword = NULL;
    const char* webhook_url = NULL;
    int opt;

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    while ((opt = getopt(argc, argv, "f:k:w:")) != -1) {
        switch (opt) {
            case 'f': log_file = optarg; break;
            case 'k': keyword = optarg; break;
            case 'w': webhook_url = optarg; break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    if (!log_file || !keyword || !webhook_url) {
        fprintf(stderr, "Error: Missing required arguments.\n");
        print_usage(argv[0]);
        return 1;
    }

    // Initialize libcurl globally
    curl_global_init(CURL_GLOBAL_ALL);

    // Compile the regex
    regex_t compiled_regex;
    int ret = regcomp(&compiled_regex, keyword, REG_EXTENDED | REG_NOSUB);
    if (ret != 0) {
        char error_msg[256];
        regerror(ret, &compiled_regex, error_msg, sizeof(error_msg));
        fprintf(stderr, "Regex compilation failed: %s\n", error_msg);
        curl_global_cleanup();
        return 1;
    }

    // Start the main tailing loop (blocks purely on inotify)
    start_tailing(log_file, &compiled_regex, keyword, webhook_url);

    regfree(&compiled_regex);
    curl_global_cleanup();

    return 0;
}
