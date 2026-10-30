#include "common.h"

#include <stdio.h>
#include <string.h>

#include "screen.h"
#include "states.h"
#include "sklaffterm_phonebook.h"
#include "sklaffterm_ui.h"

static void
draw_phonebook_frame(const int left, const int top,
                     const int right, const int bottom)
{
    screen_put_color_char_yx(top, left,
                             L'\x250C',
                             Q_COLOR_SKLAFFTERM_BORDER);
    screen_put_color_hline_yx(top, left + 1,
                              L'\x2500',
                              right - left - 1,
                              Q_COLOR_SKLAFFTERM_BORDER);
    screen_put_color_char_yx(top, right,
                             L'\x2510',
                             Q_COLOR_SKLAFFTERM_BORDER);

    screen_put_color_vline_yx(top + 1, left,
                              L'\x2502',
                              bottom - top - 1,
                              Q_COLOR_SKLAFFTERM_BORDER);
    screen_put_color_vline_yx(top + 1, right,
                              L'\x2502',
                              bottom - top - 1,
                              Q_COLOR_SKLAFFTERM_BORDER);

    screen_put_color_char_yx(bottom, left,
                             L'\x2514',
                             Q_COLOR_SKLAFFTERM_BORDER);
    screen_put_color_hline_yx(bottom, left + 1,
                              L'\x2500',
                              right - left - 1,
                              Q_COLOR_SKLAFFTERM_BORDER);
    screen_put_color_char_yx(bottom, right,
                             L'\x2518',
                             Q_COLOR_SKLAFFTERM_BORDER);

    screen_put_color_str_yx(top, left + 2,
                            " Phone book ",
                            Q_COLOR_SKLAFFTERM_BORDER);
}

static int selected_entry = 0;


void
sklaffterm_keyboard_handler(const int keystroke, const int flags)
{
    /*
     * Exit shortcuts on the start screen.
     */
    if (keystroke == Q_KEY_ESCAPE) {
        switch_state(Q_STATE_EXIT);
        return;
    }

    if ((flags & KEY_FLAG_ALT) &&
        ((keystroke == 'x') || (keystroke == 'X') ||
         (keystroke == 'q') || (keystroke == 'Q'))) {

        switch_state(Q_STATE_EXIT);
        return;
    }

    /*
     * Alt-M is reserved for the SklaffTERM menu.
     * We do not open anything yet.
     */
    if ((flags & KEY_FLAG_ALT) &&
        ((keystroke == 'm') || (keystroke == 'M'))) {

        return;
    }

    switch (keystroke) {

    case Q_KEY_UP:
        if (sklaffterm_phonebook_count() > 0) {
            selected_entry--;
            if (selected_entry < 0) {
                selected_entry =
                    (int) sklaffterm_phonebook_count() - 1;
            }
        }
        break;

    case Q_KEY_DOWN:
        if (sklaffterm_phonebook_count() > 0) {
            selected_entry++;
            if (selected_entry >=
                (int) sklaffterm_phonebook_count()) {

                selected_entry = 0;
            }
        }
        break;

    case 'q':
    case 'Q':
        switch_state(Q_STATE_EXIT);
        break;

    default:
        break;
    }
}


void
sklaffterm_refresh(void)
{
    int height;
    int width;
    int left;
    int right;
    int top;
    int bottom;
    int row;
    int i;
    Q_COLOR color;
    const SKLAFFTERM_PHONE_ENTRY *entry;
    size_t phonebook_count;
    const char *status_line =
        "Alt-A Add | Alt-E Edit | Alt-D Delete | Alt-M Menu";
    int status_x;

    screen_get_dimensions(&height, &width);
    screen_clear();

    phonebook_count = sklaffterm_phonebook_count();

    /*
     * Keep the first UI simple, but don't explode in a very small terminal.
     */
    if ((width < 60) || (height < 14)) {
        screen_put_color_str_yx(2, 2,
                                "SklaffTERM",
                                Q_COLOR_CONSOLE_TEXT);
        screen_put_color_str_yx(4, 2,
                                "Terminal window is too small.",
                                Q_COLOR_CONSOLE_TEXT);
        screen_put_color_str_yx(6, 2,
                                "Press Q to quit.",
                                Q_COLOR_CONSOLE_TEXT);
        screen_flush();
        return;
    }

    left = 2;
    right = width - 3;
    top = 3;
    bottom = top + (int) phonebook_count + 4;

    /*
     * Header.
     */
    screen_put_color_str_yx(1, left,
                            "SklaffTERM",
                            Q_COLOR_SKLAFFTERM_TEXT);

    /*
     * BBS list.
     */
    draw_phonebook_frame(left, top, right, bottom);

    row = top + 2;

    for (i = 0; i < (int) phonebook_count; i++) {

        entry = sklaffterm_phonebook_get((size_t) i);

        if (entry == NULL) {
            continue;
        }

        if (i == selected_entry) {
            color = Q_COLOR_SKLAFFTERM_SELECTED;
        } else {
            color = Q_COLOR_SKLAFFTERM_TEXT;
        }

        screen_put_color_printf_yx(
            row,
            left + 2,
            color,
            "%c %-32s %-8s %-8s",
            (i == selected_entry ? '>' : ' '),
            entry->name,
            sklaffterm_protocol_to_string(entry->protocol),
            sklaffterm_encoding_to_string(entry->encoding)
        );

        row++;
    }

    /*
     * Footer.
     *
     * Enter is the implicit/default action. Keep the secondary commands
     * grouped at the right edge.
     */
    status_x = width - 2 - (int) strlen(status_line);
    if (status_x < left) {
        status_x = left;
    }

    screen_put_color_str_yx(
        height - 2,
        status_x,
        status_line,
        Q_COLOR_SKLAFFTERM_TEXT
    );

    screen_flush();
}
