#define _XOPEN_SOURCE_EXTENDED
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include <xc_win.h>
#include <sys/queue.h>
#include <locale.h>
#include <xc_u8.h>

struct winlist_entry *lwin, *rwin;


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
        win_draw(rwin);
        int x, y;

        curs_set(0);
        int c;
        win_redraw_list();
        refresh();
        while ((c = win_getch()) != 'q') {
                switch(c) {
                case 'n':
                        win_focus_next();
                        break;
                case 'p':
                        win_focus_prev();
                        break;
                default:
                        break;
                }
                win_redraw_list();
                refresh();
        }

        win_destroy(rwin);
        win_destroy(lwin);

        endwin();

        return EXIT_SUCCESS;
}
