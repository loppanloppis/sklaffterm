#include "common.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "sklaffterm_config.h"


SKLAFFTERM_CONFIG sklaffterm_config;


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


static char *
default_transfer_dir(void)
{
    const char *home;
    char *path;
    size_t length;

    home = getenv("HOME");

    if ((home == NULL) || (home[0] == '\0')) {
        home = ".";
    }

    length = strlen(home) + strlen("/Downloads") + 1;

    path = Xmalloc(length, __FILE__, __LINE__);
    snprintf(path, length, "%s/Downloads", home);

    return path;
}


static char *
config_base_dir(void)
{
    const char *xdg;
    const char *home;
    char *path;
    size_t length;

    xdg = getenv("XDG_CONFIG_HOME");

    if ((xdg != NULL) && (xdg[0] != '\0')) {
        return string_dup(xdg);
    }

    home = getenv("HOME");

    if ((home == NULL) || (home[0] == '\0')) {
        home = ".";
    }

    length = strlen(home) + strlen("/.config") + 1;

    path = Xmalloc(length, __FILE__, __LINE__);
    snprintf(path, length, "%s/.config", home);

    return path;
}


static char *
config_dir(void)
{
    char *base;
    char *path;
    size_t length;

    base = config_base_dir();

    length = strlen(base) + strlen("/sklaffterm") + 1;

    path = Xmalloc(length, __FILE__, __LINE__);
    snprintf(path, length, "%s/sklaffterm", base);

    Xfree(base, __FILE__, __LINE__);

    return path;
}


static char *
config_filename(void)
{
    char *dir;
    char *path;
    size_t length;

    dir = config_dir();

    length = strlen(dir) + strlen("/config") + 1;

    path = Xmalloc(length, __FILE__, __LINE__);
    snprintf(path, length, "%s/config", dir);

    Xfree(dir, __FILE__, __LINE__);

    return path;
}


static int
mkdir_if_needed(const char *path)
{
    if (mkdir(path, 0700) == 0) {
        return 0;
    }

    if (errno == EEXIST) {
        return 0;
    }

    return -1;
}


static int
ensure_config_dir(void)
{
    char *base;
    char *dir;

    base = config_base_dir();

    if (mkdir_if_needed(base) != 0) {
        fprintf(stderr,
                "sklaffterm: cannot create config directory %s: %s\n",
                base, strerror(errno));
        Xfree(base, __FILE__, __LINE__);
        return -1;
    }

    Xfree(base, __FILE__, __LINE__);

    dir = config_dir();

    if (mkdir_if_needed(dir) != 0) {
        fprintf(stderr,
                "sklaffterm: cannot create config directory %s: %s\n",
                dir, strerror(errno));
        Xfree(dir, __FILE__, __LINE__);
        return -1;
    }

    Xfree(dir, __FILE__, __LINE__);

    return 0;
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


static SKLAFF_BAUD
baud_from_string(const char *value)
{
    if (strcmp(value, "300") == 0) {
        return SKLAFF_BAUD_300;
    }

    if (strcmp(value, "1200") == 0) {
        return SKLAFF_BAUD_1200;
    }

    if (strcmp(value, "2400") == 0) {
        return SKLAFF_BAUD_2400;
    }

    if (strcmp(value, "9600") == 0) {
        return SKLAFF_BAUD_9600;
    }

    if (strcmp(value, "14400") == 0) {
        return SKLAFF_BAUD_14400;
    }

    if (strcmp(value, "19200") == 0) {
        return SKLAFF_BAUD_19200;
    }

    if (strcmp(value, "28800") == 0) {
        return SKLAFF_BAUD_28800;
    }

    if (strcmp(value, "33600") == 0) {
        return SKLAFF_BAUD_33600;
    }

    if (strcmp(value, "56000") == 0) {
        return SKLAFF_BAUD_56000;
    }

    return SKLAFF_BAUD_UNLIMITED;
}


const char *
sklaffterm_baud_to_string(SKLAFF_BAUD baud)
{
    switch (baud) {

    case SKLAFF_BAUD_300:
        return "300";

    case SKLAFF_BAUD_1200:
        return "1200";

    case SKLAFF_BAUD_2400:
        return "2400";

    case SKLAFF_BAUD_9600:
        return "9600";

    case SKLAFF_BAUD_14400:
        return "14400";

    case SKLAFF_BAUD_19200:
        return "19200";

    case SKLAFF_BAUD_28800:
        return "28800";

    case SKLAFF_BAUD_33600:
        return "33600";

    case SKLAFF_BAUD_56000:
        return "56000";

    case SKLAFF_BAUD_UNLIMITED:
    default:
        return "unlimited";
    }
}


void
sklaffterm_config_init(void)
{
    sklaffterm_config.download_dir = default_transfer_dir();
    sklaffterm_config.upload_dir = default_transfer_dir();
    sklaffterm_config.baud = SKLAFF_BAUD_UNLIMITED;
}


void
sklaffterm_config_load(void)
{
    FILE *file;
    char *filename;
    char line[1024];

    filename = config_filename();

    file = fopen(filename, "r");

    if (file == NULL) {
        if (errno == ENOENT) {
            Xfree(filename, __FILE__, __LINE__);
            sklaffterm_config_save();
            return;
        }

        fprintf(stderr,
                "sklaffterm: cannot read config %s: %s\n",
                filename, strerror(errno));

        Xfree(filename, __FILE__, __LINE__);
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *key;
        char *value;
        char *separator;

        key = trim_whitespace(line);

        if ((*key == '\0') || (*key == '#')) {
            continue;
        }

        separator = strchr(key, '=');

        if (separator == NULL) {
            continue;
        }

        *separator = '\0';

        value = trim_whitespace(separator + 1);
        key = trim_whitespace(key);

        if (strcmp(key, "download_dir") == 0) {
            if (*value != '\0') {
                replace_string(&sklaffterm_config.download_dir, value);
            }
            continue;
        }

        if (strcmp(key, "upload_dir") == 0) {
            if (*value != '\0') {
                replace_string(&sklaffterm_config.upload_dir, value);
            }
            continue;
        }

        if (strcmp(key, "baud") == 0) {
            sklaffterm_config.baud = baud_from_string(value);
            continue;
        }
    }

    fclose(file);
    Xfree(filename, __FILE__, __LINE__);
}


void
sklaffterm_config_save(void)
{
    FILE *file;
    char *filename;

    if (ensure_config_dir() != 0) {
        return;
    }

    filename = config_filename();

    file = fopen(filename, "w");

    if (file == NULL) {
        fprintf(stderr,
                "sklaffterm: cannot write config %s: %s\n",
                filename, strerror(errno));

        Xfree(filename, __FILE__, __LINE__);
        return;
    }

    fprintf(file,
            "# SklaffTERM configuration\n"
            "\n"
            "download_dir=%s\n"
            "upload_dir=%s\n"
            "baud=%s\n",
            sklaffterm_config.download_dir,
            sklaffterm_config.upload_dir,
            sklaffterm_baud_to_string(sklaffterm_config.baud));

    fclose(file);
    Xfree(filename, __FILE__, __LINE__);
}


void
sklaffterm_config_free(void)
{
    if (sklaffterm_config.download_dir != NULL) {
        Xfree(sklaffterm_config.download_dir, __FILE__, __LINE__);
        sklaffterm_config.download_dir = NULL;
    }

    if (sklaffterm_config.upload_dir != NULL) {
        Xfree(sklaffterm_config.upload_dir, __FILE__, __LINE__);
        sklaffterm_config.upload_dir = NULL;
    }
}
