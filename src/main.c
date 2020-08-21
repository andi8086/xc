#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include <xc_win.h>
#include <sys/queue.h>

struct winlist_entry *g_window, *w2, *w3;


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
	const char *msg = "Press 'q' to quit, 'n': next, 'p': prev window";

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
	g_window = win_create(15, 8, 3, 3);
	w2 = win_create(10, 20, 5, 5);
	w3 = win_create(13, 13, 6, 7);
	//win_set_color(g_window, 5);
	//win_set_color(w2, 6);
	win_set_color(w3, 3);

	int x, y;

	x = (COLS - strlen(msg)) / 2 - 1;
	y = LINES / 2 - 1;
	mvaddstr(y, x, msg);
	refresh();

	int c;
	while ((c = win_getch()) != 'q') {
		if (c == 'n') {
			win_focus_next();
		}
		if (c == 'p') {
			win_focus_prev();
		}
		win_redraw_list();
		refresh();
	}

	win_destroy(g_window);
	win_destroy(w2);
	win_destroy(w3);

	endwin();

	return EXIT_SUCCESS;
}
