#include <stdlib.h>
#include <xc_win.h>

LIST_HEAD(swinlist_head, winlist_entry) winlist_head =
	LIST_HEAD_INITIALIZER(winlist_head);


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
	LIST_INSERT_HEAD(&winlist_head, we, entries);

	we->has_focus = false;

	return we;
}


void win_destroy(struct winlist_entry *we)
{
	
	if (!we || !we->w || !we->w->win) {
		return;
	}
	delwin(we->w->win);
	free(we->w);
	
	LIST_REMOVE(we, entries);	
	free(we);

	refresh();
}


void win_updatelist(void)
{
	struct winlist_entry *we;
	WINDOW *focused_win;

	focused_win = NULL;

	LIST_FOREACH(we, &winlist_head, entries) {
		if (!we->has_focus) {
			wrefresh(we->w->win);	
		} else {
			focused_win = we->w->win;
		}
	}	

	if (focused_win) {			
		wrefresh(focused_win);
	}	
}
