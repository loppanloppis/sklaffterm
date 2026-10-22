#include "common.h"
#include "screen.h"
#include "states.h"
#include "sklaffterm_ui.h"

void
sklaffterm_keyboard_handler(const int keystroke, const int flags)
{
    (void) flags;

    if ((keystroke == 'q') || (keystroke == 'Q')) {
        switch_state(Q_STATE_EXIT);
    }
}

void
sklaffterm_refresh(void)
{
    screen_clear();

    screen_put_color_str_yx(2, 2,
                            "SklaffTERM",
                            Q_COLOR_CONSOLE_TEXT);

    screen_put_color_str_yx(4, 2,
                            "Press Q to quit",
                            Q_COLOR_CONSOLE_TEXT);

    screen_flush();
}
