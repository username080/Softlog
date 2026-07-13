#ifndef WEBHOOK_H
#define WEBHOOK_H

#include <stdbool.h>

// Sends an HTTPS POST request with the given JSON payload to the webhook URL.
bool send_webhook(const char* webhook_url, const char* json_payload);

#endif // WEBHOOK_H
