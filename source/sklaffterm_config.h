#ifndef SKLAFFTERM_CONFIG_H
#define SKLAFFTERM_CONFIG_H

typedef enum {
    SKLAFF_BAUD_UNLIMITED = 0,
    SKLAFF_BAUD_300,
    SKLAFF_BAUD_1200,
    SKLAFF_BAUD_2400,
    SKLAFF_BAUD_9600,
    SKLAFF_BAUD_14400,
    SKLAFF_BAUD_19200,
    SKLAFF_BAUD_28800,
    SKLAFF_BAUD_33600,
    SKLAFF_BAUD_56000
} SKLAFF_BAUD;

typedef struct {
    char *download_dir;
    char *upload_dir;
    SKLAFF_BAUD baud;
} SKLAFFTERM_CONFIG;

extern SKLAFFTERM_CONFIG sklaffterm_config;

void sklaffterm_config_init(void);
void sklaffterm_config_free(void);

#endif
