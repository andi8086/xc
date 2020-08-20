#include <stdlib.h>
#include <xc_win.h>


XC_WIN *win_create(int height, int width, int y, int x)
{
	XC_WIN *tmp;

	tmp = (XC_WIN *)malloc(sizeof(XC_WIN));
	if (!tmp) {
		return NULL;
	}	

	tmp->win = newwin(height, width, y, x);
	if (!tmp->win) {
		free(tmp);
		return NULL;
	}

	tmp->x = x;
	tmp->y = y;
	tmp->h = height;
	tmp->w = width;
	box(tmp->win, 0, 0);
	wrefresh(tmp->win);
	return tmp;
}


void win_destroy(XC_WIN *xc_win)
{
	if (!xc_win || !xc_win->win) {
		return;
	}
	delwin(xc_win->win);
	free(xc_win);
	refresh();
}

