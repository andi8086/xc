#ifndef __XC_WIN_H__
#define __XC_WIN_H__

#include <xc.h>

typedef struct {
	char *name;	/* name UTF-8 */
	int x, y;	/* location */
	int w, h;	/* dimensions */
	WINDOW *win;	/* ncurses window handle */
} XC_WIN;

XC_WIN *win_create(int height, int width, int y, int x);
void win_destroy(XC_WIN *xc_win);

#endif
