#include <stdlib.h>
#include <xc_win.h>
#include <xc_u8.h>
#include <xc_render.h>
#include <string.h>


LIST_HEAD(winlist_head, winlist_entry) winlist_head =
        LIST_HEAD_INITIALIZER(winlist_head);

struct winlist_head *headp;

static struct winlist_entry *focused_win;

static void win_draw_title(struct winlist_entry *w);


void win_non_focusable(struct winlist_entry *w)
{
        w->focusable = false;
}


bool win_handle_input(struct winlist_entry *w, int key)
{
        if (!w) {
                return false;
        }

        return render_infos[w->w->rmode].render_input_cb(w, key);
}


struct winlist_entry *win_get_focused(void)
{
        return focused_win;
}


int win_set_render_mode(struct winlist_entry *w, xc_render_mode_t rmode)
{
        int res;
        res = render_infos[rmode].render_init(&(w->w->render_ctx));
        if (res) {
                return res;
        }
        w->w->rmode = rmode;
        return 0;
}


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
        keypad(we->w->win, TRUE); /* interpret function keys */

        we->focusable = true;
        wtimeout(we->w->win, 10);
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
        struct winlist_entry *we, *curr_focus;

        focused_win->has_focus = false;
        win_draw(focused_win);

        curr_focus = focused_win;

        /* repeat as long as we don't have a focusable window, until we get
         * to the same that already had the focus */
        do {
                we = LIST_NEXT(focused_win, entries);
                if (we) {
                        focused_win = we;
                } else {
                        we = LIST_FIRST(&winlist_head);
                        if (we) {
                                focused_win = we;
                        }
                }
        } while (focused_win != curr_focus &&
                 !focused_win->focusable);

        focused_win->has_focus = true;
}


void win_focus_prev(void)
{
        struct winlist_entry *we, *tmp, *curr_focus;

        focused_win->has_focus = false;
        win_draw(focused_win);

        curr_focus = focused_win;

        do {
                we = LIST_PREV(focused_win, &winlist_head, winlist_entry,
                               entries);
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
        } while (focused_win != curr_focus &&
                 !focused_win->focusable);

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


static void win_draw_title(struct winlist_entry *w)
{
        if (!w->w->title) {
                return;
        }
        char *dtitle = strabbrev_u8(w->w->title, w->w->w);
        if (!dtitle) {
                return;
        }
        if (w->has_focus) {
                wattron(w->w->win, COLOR_PAIR(6));
        }
        mvwaddstr(w->w->win,
                0, (w->w->w - mbswidth(dtitle))/2, dtitle);
        free(dtitle);
        if (w->has_focus) {
                wattroff(w->w->win, COLOR_PAIR(6));
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
        switch (w->w->border) {
        case BORDER_SINGLE:
                box(w->w->win, 0, 0);
                break;
        case BORDER_DOUBLE:
                wborder_set(w->w->win, WACS_D_VLINE, WACS_D_VLINE,
                        WACS_D_HLINE, WACS_D_HLINE,
                        WACS_D_ULCORNER, WACS_D_URCORNER,
                        WACS_D_LLCORNER, WACS_D_LRCORNER);
                break;
        case BORDER_NONE:
        default:
                break;

        }

        win_draw_title(w);
        render_infos[w->w->rmode].render_draw(w, w->w->w-2, w->w->h-2);
}


void win_set_border(struct winlist_entry *w, win_border_t border)
{
        if (!w || !w->w) {
                return;
        }
        w->w->border = border;
}

