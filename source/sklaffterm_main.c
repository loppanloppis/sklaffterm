#include "qodem.h"
#include "states.h"

int
main(int argc, char * const argv[])
{
    q_startup_state = Q_STATE_SKLAFFTERM;

    return qodem_main(argc, argv);
}
