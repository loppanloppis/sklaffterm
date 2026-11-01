#include "qodem.h"
#include "states.h"
#include "sklaffterm_config.h"
#include "sklaffterm_phonebook.h"
#include "options.h"
#include "sklaffterm_adapter.h"

int
main(int argc, char * const argv[])
{
    int rc;

    sklaffterm_config_init();
    sklaffterm_config_load();

    sklaffterm_phonebook_init();
    sklaffterm_phonebook_load();

    q_options_loaded_hook = sklaffterm_adapter_apply_config;
    q_startup_state = Q_STATE_SKLAFFTERM;

    rc = qodem_main(argc, argv);

    sklaffterm_phonebook_free();
    sklaffterm_config_free();

    return rc;
}
