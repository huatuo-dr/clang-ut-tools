#include <stdio.h>
#include <string.h>
#include "greeting.h"

const char* say_hello(const char* name) {
    static char buffer[256];

    if (name == NULL || strlen(name) == 0) {
        snprintf(buffer, sizeof(buffer), "Hello, stranger!");
    } else {
        snprintf(buffer, sizeof(buffer), "Hello, %s!", name);
    }

    return buffer;
}

const char* say_goodbye(const char* name) {
    static char buffer[256];

    if (name == NULL || strlen(name) == 0) {
        snprintf(buffer, sizeof(buffer), "Goodbye, stranger!");
    } else {
        snprintf(buffer, sizeof(buffer), "Goodbye, %s!", name);
    }

    return buffer;
}
