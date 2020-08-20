#include <stdlib.h>
#include <xc_win.h>

SLIST_HEAD(swinlist_head, winlist_entry) winlist_head =
	SLIST_HEAD_INITIALIZER(winlist_head);


struct winlist_entry *win_create(int height, int width, int y, int x)
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

	struct winlist_entry *we;

	we = malloc(sizeof(struct winlist_entry));
	if (!we) {
		delwin(tmp->win);
		free(tmp);
		return NULL;
	}

	tmp->x = x;
	tmp->y = y;
	tmp->h = height;
	tmp->w = width;
	box(tmp->win, 0, 0);
	wrefresh(tmp->win);

	we->w = tmp;
	SLIST_INSERT_HEAD(&winlist_head, we, entries);

	return we;
}


void win_destroy(struct winlist_entry *we)
{
	
	if (!we || !we->w || !we->w->win) {
		return;
	}
	delwin(we->w->win);
	free(we->w);
	
	SLIST_REMOVE(&winlist_head, we, winlist_entry, entries);	
	free(we);

	refresh();
}

