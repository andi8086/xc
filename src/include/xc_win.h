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


struct winlist_entry *win_create(int height, int width, int y, int x);
void win_destroy(struct winlist_entry *we);

#endif
