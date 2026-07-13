#ifndef JSON_H
#define JSON_H

#include <stddef.h>
#include <stdbool.h>

// Escapes a string for safe inclusion inside a JSON string value.
// Returns false if the escaped string would exceed max_out_len.
bool escape_json_string(const char* input, char* output, size_t max_out_len);

#endif // JSON_H
