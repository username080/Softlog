#include "reader.h"
#include "../regex/matcher.h"
#include "../webhook/sender.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>
#include <errno.h>

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#define MAX_SEC 60
#include "../utils/json.h"

// Helper to open the file and seek to the end
static FILE* open_and_seek_end(const char* filepath) {
    FILE* file = NULL;
    int count = 0;
    while (!file) {
        file = fopen(filepath, "r");
        if(count == (MAX_SEC * 10)){
            fprintf(stderr,"File could not  be found\n");
            return NULL;
        }
        if (!file) {
            // File might not exist yet during a rapid rotation, wait and retry
            count ++;
            usleep(100000); // 100ms
        
        }
    }
    fseek(file, 0, SEEK_END);
    return file;
}

// Helper to read all available new lines
static void read_available_lines(FILE* file, regex_t* compiled_regex, const char* pattern_str, const char* webhook_url) {
    char line_buffer[4096];
    char json_payload[16384];
    char escaped_line[8192];
    char escaped_pattern[1024];

    // Escape the pattern once
    escape_json_string(pattern_str, escaped_pattern, sizeof(escaped_pattern));

    clearerr(file);
    while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
        size_t len = strlen(line_buffer);
        bool newline_found = false;
        
        if (len > 0 && line_buffer[len - 1] == '\n') {
            line_buffer[len - 1] = '\0';
            newline_found = true;
        }

        // Buffer overflow protection: If no newline was found and we filled the buffer,
        // it means the line is longer than 4095 bytes!
        // We must flush the rest of the line from the stream so it doesn't corrupt the next read.
        if (!newline_found && len == sizeof(line_buffer) - 1) {
            int ch;
            while ((ch = fgetc(file)) != EOF && ch != '\n') {
                // skip characters
            }
        }

        if (check_log_line(line_buffer, compiled_regex)) {
            printf("MATCH FOUND: %s\n", line_buffer);
            
            if (escape_json_string(line_buffer, escaped_line, sizeof(escaped_line))) {
                snprintf(json_payload, sizeof(json_payload), 
                         "{\"content\":\"**Softlog Alert!**\\nMatched Pattern: `%s`\\nLog Line: `%s`\"}", 
                         escaped_pattern, escaped_line);
                send_webhook(webhook_url, json_payload);
            } else {
                fprintf(stderr, "Failed to escape log line for JSON!\n");
            }
        }
    }
}

void start_tailing(const char* filepath, regex_t* compiled_regex, const char* pattern_str, const char* webhook_url) {
    printf("Watching %s for '%s'...\n", filepath, pattern_str);

    int inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        perror("inotify_init");
        return;
    }

    FILE* file = open_and_seek_end(filepath);
    if(!file) return;
    // We watch for MODIFY (new logs), MOVE_SELF (logrotate mv), and DELETE_SELF
    int wd = inotify_add_watch(inotify_fd, filepath, IN_MODIFY | IN_MOVE_SELF | IN_DELETE_SELF);
    if (wd == -1) {
        perror("inotify_add_watch");
    }

    char buf[BUF_LEN] __attribute__ ((aligned(8)));

    while (1) {
        // Block perfectly at the kernel level until an event happens (0% CPU)
        ssize_t numRead = read(inotify_fd, buf, BUF_LEN);
        if (numRead <= 0) {
            perror("read inotify");
            sleep(1);
            continue;
        }

        for (char *ptr = buf; ptr < buf + numRead; ) {
            struct inotify_event *event = (struct inotify_event *) ptr;

            if (event->mask & IN_MODIFY) {
                // Read the new lines
                read_available_lines(file, compiled_regex, pattern_str, webhook_url);
            }

            if ((event->mask & IN_MOVE_SELF) || (event->mask & IN_DELETE_SELF)) {
                // Rotation detected! 
                printf("[Watchdog] Log rotation detected via event! File moved/deleted.\n");
                
                // Remove old watch safely
                inotify_rm_watch(inotify_fd, wd);
                fclose(file);
                
                // Block until the new file is created by logrotate
                file = open_and_seek_end(filepath);
                
                // Add the watch to the brand new inode
                wd = inotify_add_watch(inotify_fd, filepath, IN_MODIFY | IN_MOVE_SELF | IN_DELETE_SELF);
                if (wd == -1) {
                    perror("inotify_add_watch new file");
                }
                printf("[Watchdog] Successfully reopened new log file.\n");
            }

            ptr += sizeof(struct inotify_event) + event->len;
        }
    }

    if (file) fclose(file);
    close(inotify_fd);
}
