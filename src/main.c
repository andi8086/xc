#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include <xc_win.h>	
#include <sys/queue.h>

struct winlist_entry *g_window;


void init_colors(void)
{
	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_CYAN, COLOR_BLACK);
	init_pair(4, COLOR_RED, COLOR_BLACK);
}


int main(int argc, char *argv[])
{
	const char *msg = "Press any key to quit";

	initscr();
	cbreak();
	noecho();

	init_colors();

	clear();
	refresh();

	g_window = win_create(LINES-1, COLS-1, 0, 0);

	int x, y;

	x = (COLS - strlen(msg)) / 2 - 1;
	y = LINES / 2 - 1;
	mvaddstr(y, x, msg);


	getch();

	win_destroy(g_window);
	endwin();

	return EXIT_SUCCESS;
}
