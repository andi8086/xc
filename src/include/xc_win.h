#ifndef __XC_WIN_H__
#define __XC_WIN_H__

#include <xc.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct _XC_WIN {
	char *title;	/* title UTF-8 */
	int x, y;	/* location */
	int w, h;	/* dimensions */
	int cp;		/* color pair */
	bool double_border;
	WINDOW *win;	/* ncurses window handle */
} XC_WIN;


struct winlist_entry {
	XC_WIN *w;
	bool has_focus;
	LIST_ENTRY(winlist_entry) entries;
};

#ifndef LIST_PREV

/* missing LIST_PREV macro */

#define __containerof(ptr, type, member) ({	\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type, member) ); })


#define LIST_PREV(elm, head, type, field)	\
	((elm)->field.le_prev == &LIST_FIRST((head)) ? NULL :	\
		__containerof((elm)->field.le_prev,		\
		struct type, field.le_next))

#endif


struct winlist_entry *win_create(int height, int width, int y, int x);
struct winlist_entry *win_create_c(int height, int width, int y, int x,
	int cp);
void win_destroy(struct winlist_entry *we);
void win_focus_next(void);
void win_focus_prev(void);
void win_redraw_list(void);
void win_set_color(struct winlist_entry *w, int cp);
int win_getch(void);
void win_set_title(struct winlist_entry *w, char *title);
void win_draw(struct winlist_entry *w);
void win_draw_titlebar(struct winlist_entry *w);
void win_set_double_border(struct winlist_entry *w, bool double_border);
#endif
