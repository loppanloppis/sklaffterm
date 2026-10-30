#ifndef SKLAFFTERM_PHONEBOOK_H
#define SKLAFFTERM_PHONEBOOK_H

#include <stddef.h>

typedef enum {
    SKLAFF_PROTOCOL_TELNET,
    SKLAFF_PROTOCOL_SSH
} SKLAFF_PROTOCOL;

typedef enum {
    SKLAFF_ENCODING_UTF8,
    SKLAFF_ENCODING_CP437,
    SKLAFF_ENCODING_CP850
} SKLAFF_ENCODING;

typedef struct {
    char *name;
    char *host;
    int port;
    SKLAFF_PROTOCOL protocol;
    char *username;
    SKLAFF_ENCODING encoding;
} SKLAFFTERM_PHONE_ENTRY;

void sklaffterm_phonebook_init(void);
void sklaffterm_phonebook_free(void);

size_t sklaffterm_phonebook_count(void);

const SKLAFFTERM_PHONE_ENTRY *
sklaffterm_phonebook_get(size_t index);

const char *
sklaffterm_protocol_to_string(SKLAFF_PROTOCOL protocol);

const char *
sklaffterm_encoding_to_string(SKLAFF_ENCODING encoding);

#endif
