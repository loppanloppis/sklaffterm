#include "common.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "sklaffterm_phonebook.h"


static SKLAFFTERM_PHONE_ENTRY *entries = NULL;
static size_t entry_count = 0;
static size_t entry_capacity = 0;


static char *
string_dup(const char *source)
{
    char *copy;
    size_t length;

    length = strlen(source) + 1;

    copy = Xmalloc(length, __FILE__, __LINE__);
    memcpy(copy, source, length);

    return copy;
}


static void
replace_string(char **target, const char *value)
{
    if (*target != NULL) {
        Xfree(*target, __FILE__, __LINE__);
    }

    *target = string_dup(value);
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


static SKLAFFTERM_PHONE_ENTRY *
append_entry(const char *name)
{
    SKLAFFTERM_PHONE_ENTRY *new_entries;
    SKLAFFTERM_PHONE_ENTRY *entry;
    size_t new_capacity;

    if (entry_count >= entry_capacity) {

        if (entry_capacity == 0) {
            new_capacity = 8;
        } else {
            new_capacity = entry_capacity * 2;
        }

        new_entries = Xmalloc(
            new_capacity * sizeof(SKLAFFTERM_PHONE_ENTRY),
            __FILE__, __LINE__
        );

        if (entries != NULL) {
            memcpy(
                new_entries,
                entries,
                entry_count * sizeof(SKLAFFTERM_PHONE_ENTRY)
            );

            Xfree(entries, __FILE__, __LINE__);
        }

        entries = new_entries;
        entry_capacity = new_capacity;
    }

    entry = &entries[entry_count];

    memset(entry, 0, sizeof(*entry));

    entry->name = string_dup(name);
    entry->host = string_dup("");
    entry->port = 23;
    entry->protocol = SKLAFF_PROTOCOL_TELNET;
    entry->username = string_dup("");
    entry->encoding = SKLAFF_ENCODING_UTF8;

    entry_count++;

    return entry;
}


static char *
trim_whitespace(char *text)
{
    char *end;

    while ((*text != '\0') && isspace((unsigned char) *text)) {
        text++;
    }

    if (*text == '\0') {
        return text;
    }

    end = text + strlen(text) - 1;

    while ((end > text) && isspace((unsigned char) *end)) {
        *end = '\0';
        end--;
    }

    return text;
}

static const char *
protocol_to_config_string(SKLAFF_PROTOCOL protocol)
{
    switch (protocol) {
    case SKLAFF_PROTOCOL_SSH:
        return "ssh";

    case SKLAFF_PROTOCOL_TELNET:
    default:
        return "telnet";
    }
}


static const char *
encoding_to_config_string(SKLAFF_ENCODING encoding)
{
    switch (encoding) {
    case SKLAFF_ENCODING_CP437:
        return "cp437";

    case SKLAFF_ENCODING_CP850:
        return "cp850";

    case SKLAFF_ENCODING_UTF8:
    default:
        return "utf8";
    }
}

static char *
phonebook_filename(void)
{
    const char *xdg;
    const char *home;
    const char *suffix;
    char *path;
    size_t length;

    xdg = getenv("XDG_CONFIG_HOME");

    if ((xdg != NULL) && (xdg[0] != '\0')) {
        suffix = "/sklaffterm/phonebook";

        length = strlen(xdg) + strlen(suffix) + 1;

        path = Xmalloc(length, __FILE__, __LINE__);
        snprintf(path, length, "%s%s", xdg, suffix);

        return path;
    }

    home = getenv("HOME");

    if ((home == NULL) || (home[0] == '\0')) {
        home = ".";
    }

    suffix = "/.config/sklaffterm/phonebook";

    length = strlen(home) + strlen(suffix) + 1;

    path = Xmalloc(length, __FILE__, __LINE__);
    snprintf(path, length, "%s%s", home, suffix);

    return path;
}


static SKLAFF_PROTOCOL
protocol_from_string(const char *value)
{
    if (strcasecmp(value, "ssh") == 0) {
        return SKLAFF_PROTOCOL_SSH;
    }

    return SKLAFF_PROTOCOL_TELNET;
}


static SKLAFF_ENCODING
encoding_from_string(const char *value)
{
    if (strcasecmp(value, "cp437") == 0) {
        return SKLAFF_ENCODING_CP437;
    }

    if (strcasecmp(value, "cp850") == 0) {
        return SKLAFF_ENCODING_CP850;
    }

    return SKLAFF_ENCODING_UTF8;
}


static int
port_from_string(const char *value, int fallback)
{
    char *end;
    long port;

    errno = 0;

    port = strtol(value, &end, 10);

    if ((errno != 0) ||
        (end == value) ||
        (*end != '\0') ||
        (port < 1) ||
        (port > 65535)) {

        return fallback;
    }

    return (int) port;
}


static void
add_default_entries(void)
{
    SKLAFFTERM_PHONE_ENTRY *entry;

    entry = append_entry("Twilight Node");
    replace_string(&entry->host, "twilightnode.se");
    entry->port = 22;
    entry->protocol = SKLAFF_PROTOCOL_SSH;
    replace_string(&entry->username, "peterlondon");
    entry->encoding = SKLAFF_ENCODING_UTF8;

    entry = append_entry("Nikom BBS");
    replace_string(&entry->host, "bbs.nikom.org");
    entry->port = 2323;
    entry->protocol = SKLAFF_PROTOCOL_TELNET;
    replace_string(&entry->username, "");
    entry->encoding = SKLAFF_ENCODING_UTF8;
}


void
sklaffterm_phonebook_init(void)
{
    entries = NULL;
    entry_count = 0;
    entry_capacity = 0;
}


void
sklaffterm_phonebook_load(void)
{
    FILE *file;
    char *filename;
    char line[1024];
    SKLAFFTERM_PHONE_ENTRY *current_entry;

    filename = phonebook_filename();

    file = fopen(filename, "r");

    if (file == NULL) {
        if (errno == ENOENT) {
            Xfree(filename, __FILE__, __LINE__);

            add_default_entries();
            sklaffterm_phonebook_save();

            return;
        }

        fprintf(stderr,
                "sklaffterm: cannot read phone book %s: %s\n",
                filename, strerror(errno));

        Xfree(filename, __FILE__, __LINE__);
        return;
    }

    current_entry = NULL;

    while (fgets(line, sizeof(line), file) != NULL) {
        char *text;
        char *separator;
        char *key;
        char *value;
        size_t length;

        text = trim_whitespace(line);

        if ((*text == '\0') || (*text == '#')) {
            continue;
        }

        length = strlen(text);

        if ((length >= 2) &&
            (text[0] == '[') &&
            (text[length - 1] == ']')) {

            text[length - 1] = '\0';
            text++;

            text = trim_whitespace(text);

            if (*text != '\0') {
                current_entry = append_entry(text);
            }

            continue;
        }

        if (current_entry == NULL) {
            continue;
        }

        separator = strchr(text, '=');

        if (separator == NULL) {
            continue;
        }

        *separator = '\0';

        key = trim_whitespace(text);
        value = trim_whitespace(separator + 1);

        if (strcmp(key, "host") == 0) {
            replace_string(&current_entry->host, value);
            continue;
        }

        if (strcmp(key, "port") == 0) {
            current_entry->port =
                port_from_string(value, current_entry->port);
            continue;
        }

        if (strcmp(key, "protocol") == 0) {
            current_entry->protocol =
                protocol_from_string(value);
            continue;
        }

        if (strcmp(key, "username") == 0) {
            replace_string(&current_entry->username, value);
            continue;
        }

        if (strcmp(key, "encoding") == 0) {
            current_entry->encoding =
                encoding_from_string(value);
            continue;
        }
    }

    fclose(file);
    Xfree(filename, __FILE__, __LINE__);
}


void
sklaffterm_phonebook_save(void)
{
    FILE *file;
    char *filename;
    size_t i;

    filename = phonebook_filename();

    file = fopen(filename, "w");

    if (file == NULL) {
        fprintf(stderr,
                "sklaffterm: cannot write phone book %s: %s\n",
                filename, strerror(errno));

        Xfree(filename, __FILE__, __LINE__);
        return;
    }

    fprintf(file,
            "# SklaffTERM phone book\n"
            "\n");

    for (i = 0; i < entry_count; i++) {

        fprintf(file,
                "[%s]\n"
                "host=%s\n"
                "port=%d\n"
                "protocol=%s\n"
                "username=%s\n"
                "encoding=%s\n"
                "\n",
                entries[i].name,
                entries[i].host,
                entries[i].port,
                protocol_to_config_string(entries[i].protocol),
                entries[i].username,
                encoding_to_config_string(entries[i].encoding));
    }

    fclose(file);
    Xfree(filename, __FILE__, __LINE__);
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
    entry_capacity = 0;
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
        return "UTF8";
    }
}
