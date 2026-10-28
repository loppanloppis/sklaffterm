#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sklaffterm_config.h"


SKLAFFTERM_CONFIG sklaffterm_config;


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


void
sklaffterm_config_init(void)
{
    sklaffterm_config.download_dir = default_transfer_dir();
    sklaffterm_config.upload_dir = default_transfer_dir();
    sklaffterm_config.baud = SKLAFF_BAUD_UNLIMITED;
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
