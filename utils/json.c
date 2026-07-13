#include "json.h"

bool escape_json_string(const char* input, char* output, size_t max_out_len) {
    if (!input || !output || max_out_len == 0) return false;

    size_t out_idx = 0;
    while (*input != '\0') {
        if (out_idx + 2 >= max_out_len) return false; // Need space for escape + char + null

        switch (*input) {
            case '"':
                output[out_idx++] = '\\';
                output[out_idx++] = '"';
                break;
            case '\\':
                output[out_idx++] = '\\';
                output[out_idx++] = '\\';
                break;
            case '\b':
                output[out_idx++] = '\\';
                output[out_idx++] = 'b';
                break;
            case '\f':
                output[out_idx++] = '\\';
                output[out_idx++] = 'f';
                break;
            case '\n':
                output[out_idx++] = '\\';
                output[out_idx++] = 'n';
                break;
            case '\r':
                output[out_idx++] = '\\';
                output[out_idx++] = 'r';
                break;
            case '\t':
                output[out_idx++] = '\\';
                output[out_idx++] = 't';
                break;
            default:
                // For simplicity, we just copy printable characters and ignore complex control chars
                if (*input >= 0x20) {
                    output[out_idx++] = *input;
                }
                break;
        }
        input++;
    }
    
    if (out_idx >= max_out_len) return false;
    output[out_idx] = '\0';
    return true;
}
