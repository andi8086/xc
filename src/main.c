#include <stdlib.h>
#include <string.h>
#include <xc.h>

int main(int argc, char *argv[])
{
	const char *msg = "Press any key to quit";


	initscr();
	cbreak();
	noecho();

	clear();

	int x, y;

	x = (COLS - strlen(msg)) / 2 - 1;
	y = LINES / 2 - 1;
	mvaddstr(y, x, msg);

	refresh();

	getch();

	endwin();

	return EXIT_SUCCESS;
}
