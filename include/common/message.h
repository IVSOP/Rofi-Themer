#ifndef MESSAGE_H
#define MESSAGE_H

#include <string.h>
#include <cstdint>

#define MESSAGE_STR_SIZE 1024

enum MESSAGE_TYPE {
    READ = 'r',
    MENU = 'm',
    SAVE = 'w',  // write
    EXIT = 'k',  // kill
    UNDEF = 'u'  // reserved for code below
};

// missing constructors etc, will add them later
struct Message {
    MESSAGE_TYPE type;
    char str[MESSAGE_STR_SIZE];

    Message()
    : type(UNDEF) {
        memset(str, '\0', MESSAGE_STR_SIZE);
    }
    ~Message() = default;
};

struct OutMessage {
    uint32_t len;
    char str[MESSAGE_STR_SIZE];

    OutMessage()
    : len(0) {
        memset(str, '\0', MESSAGE_STR_SIZE);
    }
    ~OutMessage() = default;
};

#endif
