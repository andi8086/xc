#include <stdlib.h>
#include <xc_win.h>


LIST_HEAD(winlist_head, winlist_entry) winlist_head =
	LIST_HEAD_INITIALIZER(winlist_head);

struct winlist_head *headp;

struct winlist_entry *focused_win;


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

	if (!focused_win) {
		focused_win = we;
		we->has_focus = true;
	}

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


void win_redraw_list(void)
{
	struct winlist_entry *we;


	LIST_FOREACH(we, &winlist_head, entries) {
		if (!we->has_focus) {
			wrefresh(we->w->win);	
			// redrawwin(we->w->win);
		}	
	}	

	if (focused_win) {			
		wrefresh(focused_win->w->win);
		redrawwin(focused_win->w->win);
	}	
}


void win_focus_next(void)
{
	struct winlist_entry *we;

	focused_win->has_focus = false;

	we = LIST_NEXT(focused_win, entries);	
	if (we) {
		focused_win = we;
	} else {
		we = LIST_FIRST(&winlist_head);
		if (we) {
			focused_win = we;
		}
	}
	
	focused_win->has_focus = true;
}


void win_focus_prev(void)
{
	struct winlist_entry *we, *tmp;

	focused_win->has_focus = false;

	we = LIST_PREV(focused_win, &winlist_head, winlist_entry, entries);
	if (we) {
		focused_win = we;
	} else {
		tmp = NULL;
		we = focused_win;
		while(we = LIST_NEXT(we, entries)) {
			tmp = we;
		};
		if (tmp) {
			focused_win = tmp;
		}
	}

	focused_win->has_focus = true;
}


int win_getch(void)
{
	if (focused_win) {
		return wgetch(focused_win->w->win);
	}
	return 0;
}
