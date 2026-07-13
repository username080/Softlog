#include "sender.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

static size_t dummy_write_cb(void *buffer, size_t size, size_t nmemb, void *userp) {
    (void)buffer;
    (void)userp;
    return size * nmemb;
}

bool send_webhook(const char* webhook_url, const char* json_payload) {
    if (!webhook_url || !json_payload) return false;

    CURL *curl;
    CURLcode res;
    bool success = false;

    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, webhook_url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dummy_write_cb);
        
        // Perform the request
        res = curl_easy_perform(curl);
        
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code >= 200 && response_code < 300) {
                success = true;
            } else {
                fprintf(stderr, "Webhook failed with HTTP Status: %ld\n", response_code);
            }
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    
    return success;
}
