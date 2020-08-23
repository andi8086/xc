#include <stdlib.h>
#include <xc_win.h>
#include <xc_u8.h>
#include <string.h>


LIST_HEAD(winlist_head, winlist_entry) winlist_head =
	LIST_HEAD_INITIALIZER(winlist_head);

struct winlist_head *headp;

struct winlist_entry *focused_win;


struct winlist_entry *win_create_c(int height, int width, int y, int x, int cp)
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
	tmp->cp = cp;
	wbkgd(tmp->win, COLOR_PAIR(tmp->cp));
	wclear(tmp->win);
	box(tmp->win, 0, 0);
	wrefresh(tmp->win);
	we->w = tmp;
	LIST_INSERT_HEAD(&winlist_head, we, entries);

	we->has_focus = false;

	if (!focused_win) {
		focused_win = we;
		we->has_focus = true;
	}

	we->w->title = NULL;
	return we;
}


struct winlist_entry *win_create(int height, int width, int y, int x)
{
	return win_create_c(height, width, y, x, 1);
}


void win_destroy(struct winlist_entry *we)
{
	if (!we || !we->w || !we->w->win) {
		return;
	}
	delwin(we->w->win);
	if (we->w->title) {
		free(we->w->title);
	}
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
		}
	}

	if (focused_win) {
		win_draw(focused_win);
	}
}


void win_focus_next(void)
{
	struct winlist_entry *we;

	focused_win->has_focus = false;
	win_draw_titlebar(focused_win);

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
	win_draw_titlebar(focused_win);

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


void win_set_color(struct winlist_entry *e, int cp)
{
	if (!e || !e->w || !e->w->win) {
		return;
	}
	e->w->cp = cp;
	wbkgd(e->w->win, COLOR_PAIR(cp));
	touchwin(e->w->win);
}


void win_set_title(struct winlist_entry *e, char *title)
{
	if (!e || !e->w) {
		return;
	}
	size_t bytelen = strlen(title) + 1;
	if (!e->w->title) {
		e->w->title = malloc(bytelen);
		if (!e->w->title) {
			return;
		}
		memcpy(e->w->title, title, bytelen);
		return;
	}
	e->w->title = realloc(e->w->title, bytelen);
	memcpy(e->w->title, title, bytelen);
}


void win_draw_titlebar(struct winlist_entry *w)
{
	if (!w || !w->w || !w->w->win) {
		return;
	}
	if (w->w->title) {
		int l = mbswidth(w->w->title);
		int b = strlen(w->w->title);
		// char *dtitle = strabbrev_u8(w->w->title, w->w->w);
		mvwaddstr(w->w->win, 0, 0, w->w->title);
	}
}


void win_draw(struct winlist_entry *w)
{
	if (!w || !w->w || !w->w->win) {
		return;
	}
	wrefresh(w->w->win);
	wbkgd(w->w->win, COLOR_PAIR(w->w->cp));
	redrawwin(w->w->win);
	box(w->w->win, 0, 0);
	if (w->w->title) {
		int l = mbswidth(w->w->title);
		int b = strlen(w->w->title);
		// char *dtitle = strabbrev_u8(w->w->title, w->w->w);
		wattron(w->w->win, COLOR_PAIR(5));
		mvwaddstr(w->w->win, 0, 0, w->w->title);
		wattroff(w->w->win, COLOR_PAIR(5));
	}
}
