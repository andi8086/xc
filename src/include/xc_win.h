#ifndef __XC_WIN_H__
#define __XC_WIN_H__

#include <xc.h>

typedef struct _XC_WIN {
	char *name;	/* name UTF-8 */
	int x, y;	/* location */
	int w, h;	/* dimensions */
	WINDOW *win;	/* ncurses window handle */
} XC_WIN;


struct winlist_entry {
	XC_WIN *w;
	bool has_focus;
	LIST_ENTRY(winlist_entry) entries;
};

#ifndef LIST_PREV

/* missing LIST_PREV macro */
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define __containerof(ptr, type, member) ({	\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type, member) ); })


#define LIST_PREV(elm, head, type, field)	\
	((elm)->field.le_prev == &LIST_FIRST((head)) ? NULL :	\
		__containerof((elm)->field.le_prev,		\
		struct type, field.le_next))

#endif


struct winlist_entry *win_create(int height, int width, int y, int x);
void win_destroy(struct winlist_entry *we);
void win_focus_next(void);
void win_focus_prev(void);
void win_redraw_list(void);
int win_getch(void);

#endif
