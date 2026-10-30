#include "common.h"

#include <stddef.h>
#include <stdlib.h>

#include "sklaffterm_phonebook.h"


static SKLAFFTERM_PHONE_ENTRY *entries = NULL;
static size_t entry_count = 0;


static void
set_entry(SKLAFFTERM_PHONE_ENTRY *entry,
          const char *name,
          const char *host,
          const int port,
          const SKLAFF_PROTOCOL protocol,
          const char *username,
          const SKLAFF_ENCODING encoding)
{
    entry->name = Xstrdup(name, __FILE__, __LINE__);
    entry->host = Xstrdup(host, __FILE__, __LINE__);
    entry->port = port;
    entry->protocol = protocol;
    entry->username = Xstrdup(username, __FILE__, __LINE__);
    entry->encoding = encoding;
}


static void
free_entry(SKLAFFTERM_PHONE_ENTRY *entry)
{
    if (entry->name != NULL) {
        Xfree(entry->name, __FILE__, __LINE__);
        entry->name = NULL;
    }

    if (entry->host != NULL) {
        Xfree(entry->host, __FILE__, __LINE__);
        entry->host = NULL;
    }

    if (entry->username != NULL) {
        Xfree(entry->username, __FILE__, __LINE__);
        entry->username = NULL;
    }
}


void
sklaffterm_phonebook_init(void)
{
    const size_t initial_count = 3;

    entries = Xcalloc(initial_count, sizeof(*entries), __FILE__, __LINE__);
    entry_count = initial_count;

    /*
     * Temporary entries until persistent phonebook loading exists.
     * Hostnames and usernames are intentionally empty for now.
     */
    set_entry(&entries[0],
              "Twilight Node",
              "",
              22,
              SKLAFF_PROTOCOL_SSH,
              "",
              SKLAFF_ENCODING_UTF8);

    set_entry(&entries[1],
              "SklaffKOM Test",
              "",
              23,
              SKLAFF_PROTOCOL_TELNET,
              "",
              SKLAFF_ENCODING_UTF8);

    set_entry(&entries[2],
              "Another BBS",
              "",
              23,
              SKLAFF_PROTOCOL_TELNET,
              "",
              SKLAFF_ENCODING_CP437);
}


void
sklaffterm_phonebook_free(void)
{
    size_t i;

    for (i = 0; i < entry_count; i++) {
        free_entry(&entries[i]);
    }

    if (entries != NULL) {
        Xfree(entries, __FILE__, __LINE__);
        entries = NULL;
    }

    entry_count = 0;
}


size_t
sklaffterm_phonebook_count(void)
{
    return entry_count;
}


const SKLAFFTERM_PHONE_ENTRY *
sklaffterm_phonebook_get(size_t index)
{
    if (index >= entry_count) {
        return NULL;
    }

    return &entries[index];
}


const char *
sklaffterm_protocol_to_string(SKLAFF_PROTOCOL protocol)
{
    switch (protocol) {

    case SKLAFF_PROTOCOL_SSH:
        return "SSH";

    case SKLAFF_PROTOCOL_TELNET:
    default:
        return "Telnet";
    }
}


const char *
sklaffterm_encoding_to_string(SKLAFF_ENCODING encoding)
{
    switch (encoding) {

    case SKLAFF_ENCODING_CP437:
        return "CP437";

    case SKLAFF_ENCODING_CP850:
        return "CP850";

    case SKLAFF_ENCODING_UTF8:
    default:
        return "UTF-8";
    }
}
