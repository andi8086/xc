#define _XOPEN_SOURCE_EXTENDED
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include <xc_win.h>
#include <sys/queue.h>
#include <locale.h>
#include <xc_u8.h>

struct winlist_entry *lwin, *rwin;
static WINDOW *null_win;

void init_colors(void)
{
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_CYAN, COLOR_BLACK);
        init_pair(4, COLOR_RED, COLOR_BLACK);

        init_pair(5, COLOR_WHITE, COLOR_BLUE);
        init_pair(6, COLOR_WHITE, COLOR_RED);
        init_pair(7, COLOR_BLACK, COLOR_GREEN);
}


int main(int argc, char *argv[])
{
        setlocale(LC_ALL, "en_US.UTF-8");

        initscr();
        cbreak();
        noecho();

        if (!has_colors()) {
                addstr("Cannot use colors.\n");
                getch();
        }
        if (!can_change_color()) {
                addstr("Cannot change colors.\n");
                getch();
        }
        init_colors();
        xc_render_init();

        clear();
        refresh();
        lwin = win_create(LINES-3, COLS/2, 0, 0);
        rwin = win_create(LINES-3, COLS/2, 0, COLS/2);
        win_set_color(lwin, 5);
        win_set_color(rwin, 5);
        win_set_double_border(lwin, true);
        win_set_double_border(rwin, true);
        win_set_title(lwin, "thisdir");
        win_set_title(rwin, "thatdir");
        win_set_render_mode(lwin, RENDERMODE_DIR_FULL);
        win_set_render_mode(rwin, RENDERMODE_DIR_FULL);

        win_draw(rwin);
        int x, y;

        curs_set(0);
        int c;

        bool running = true;

        win_redraw_list();
        while (running) {
                refresh();
                win_redraw_list();
                c = wgetch(win_get_focused()->w->win);

                if (win_handle_input(win_get_focused(), c)) {
                        /* input consumed by focused window */
                        continue;
                }

                switch(c) {
                case 0x09:
                        win_focus_next();
                        continue;
                case KEY_F(10):
                        running = false;
                        break;
                default:
                        break;
                }
        }

        win_destroy(rwin);
        win_destroy(lwin);

        endwin();

        return EXIT_SUCCESS;
}
