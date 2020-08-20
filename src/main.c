#include <stdlib.h>
#include <string.h>
#include <xc.h>
	

WINDOW *g_window;


WINDOW *win_create(int height, int width, int y, int x)
{
	WINDOW *win;
	win = newwin(height, width, y, x);
	box(win, 0, 0);
	wrefresh(win);
	return win;
}


void win_destroy(WINDOW *win)
{
	delwin(win);
	refresh();
}



int main(int argc, char *argv[])
{
	const char *msg = "Press any key to quit";

	initscr();
	cbreak();
	noecho();

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
