#define _XOPEN_SOURCE_EXTENDED
#include <xc.h>
#include <xc_win.h>
#include <xc_u8.h>
#include <xc_timer.h>

#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <locale.h>
#include <signal.h>

struct winlist_entry *lwin, *rwin, *fun_keys, *cmdline;
static WINDOW *null_win;


void init_colors(void)
{
        start_color();
        init_color(COLOR_YELLOW, 1000, 1000, 0);
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_CYAN, COLOR_BLACK);
        init_pair(4, COLOR_RED, COLOR_BLACK);

        init_pair(5, COLOR_WHITE, COLOR_BLUE);
        init_pair(6, COLOR_BLACK, COLOR_CYAN);
        init_pair(7, COLOR_BLACK, COLOR_GREEN);
        init_pair(8, COLOR_WHITE, COLOR_BLACK);

        init_pair(9, COLOR_WHITE, COLOR_YELLOW);
        init_color(9, 800, 0, 200);
        init_pair(10, COLOR_YELLOW, COLOR_BLUE);
        init_pair(11, COLOR_WHITE, 9);
        init_pair(12, COLOR_YELLOW, COLOR_BLACK);
}


void gui_resize(void)
{
        int max_y, max_x;
//        getmaxyx(stdscr, max_y, max_x);
        max_y = LINES;
        max_x = COLS;

        win_resize(lwin, max_y-3, max_x/2);
        win_move(lwin, 0, 0);
        win_resize(rwin, max_y-3, max_x/2);
        win_move(rwin, 0, max_x/2);

        win_resize(fun_keys, 1, max_x);
        win_move(fun_keys, max_y-1, 0);

        win_resize(cmdline, 2, max_x);
        win_move(cmdline, max_y-3, 0);
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
        win_set_border(lwin, BORDER_DOUBLE);
        win_set_border(rwin, BORDER_DOUBLE);
        win_set_render_mode(lwin, RENDERMODE_DIR_FULL);
        win_set_render_mode(rwin, RENDERMODE_DIR_FULL);
        fun_keys = win_create(1, COLS, LINES-1, 0);
        win_non_focusable(fun_keys);
        win_set_border(fun_keys, BORDER_NONE);
        win_set_render_mode(fun_keys, RENDERMODE_KEYS);
        cmdline = win_create(2, COLS, LINES-3, 0);
        win_non_focusable(cmdline);
        win_set_border(cmdline, BORDER_NONE);
        win_set_render_mode(cmdline, RENDERMODE_CMDLINE);

        win_draw(cmdline);
        win_draw(fun_keys);
        win_draw(rwin);

        if (xc_timer_init()) {
                goto error_init;
        }
        int x, y;

        curs_set(0);
        int c;

        bool running = true;

        while (running) {
                refresh();
                win_redraw_list(false);
                win_draw(cmdline);
                /* wgetch returns after timeout */

                win_draw(fun_keys);

                c = win_getch();

                if (win_handle_input(win_get_focused(), c)) {
                        /* input consumed by focused window */
                        continue;
                }
                if (win_handle_input(cmdline, c)) {
                        continue;
                }
                switch(c) {
                case '\t':
                        win_focus_next();
                        continue;
                case KEY_F(10):
                        running = false;
                        break;
                case KEY_RESIZE:
                        gui_resize();
                        win_redraw_list(true);
                        break;
                default:
                        break;
                }
        }
error_init:
        win_destroy(fun_keys);
        win_destroy(rwin);
        win_destroy(lwin);

        endwin();

        return EXIT_SUCCESS;
}
