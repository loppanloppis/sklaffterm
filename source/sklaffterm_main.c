#include "qodem.h"
#include "states.h"
#include "sklaffterm_config.h"
#include "sklaffterm_phonebook.h"

int
main(int argc, char * const argv[])
{
    int rc;

    sklaffterm_config_init();
    sklaffterm_config_load();

    sklaffterm_phonebook_init();

    q_startup_state = Q_STATE_SKLAFFTERM;

    rc = qodem_main(argc, argv);

    sklaffterm_phonebook_free();
    sklaffterm_config_free();

    return rc;
}
