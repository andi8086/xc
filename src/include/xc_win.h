#ifndef __XC_WIN_H__
#define __XC_WIN_H__

#include <xc.h>
#include <stddef.h>
#include <stdbool.h>
#include <xc_render.h>


typedef enum {
        BORDER_SINGLE = 0,
        BORDER_DOUBLE,
        BORDER_NONE
} win_border_t;


typedef struct _XC_WIN {
        char *title;    /* title UTF-8 */
        int x, y;       /* location */
        int w, h;       /* dimensions */
        int cp;         /* color pair */
        win_border_t border;
        WINDOW *win;    /* ncurses window handle */
        xc_render_mode_t rmode;
        void *render_ctx;
} XC_WIN;


struct winlist_entry {
        XC_WIN *w;
        bool has_focus;
        bool focusable;
        LIST_ENTRY(winlist_entry) entries;
};

#ifndef LIST_PREV

/* missing LIST_PREV macro */

#define __containerof(ptr, type, member) ({     \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type, member) ); })


#define LIST_PREV(elm, head, type, field)       \
        ((elm)->field.le_prev == &LIST_FIRST((head)) ? NULL :   \
                __containerof((elm)->field.le_prev,             \
                struct type, field.le_next))

#endif


struct winlist_entry *win_create(int height, int width, int y, int x);
struct winlist_entry *win_create_c(int height, int width, int y, int x,
        int cp);
void win_destroy(struct winlist_entry *we);
void win_focus_next(void);
void win_focus_prev(void);
struct winlist_entry *win_get_focused(void);
void win_redraw_list(bool force);
void win_set_color(struct winlist_entry *w, int cp);
int win_getch(void);
void win_set_title(struct winlist_entry *w, char *title);
void win_draw(struct winlist_entry *w);
void win_draw_titlebar(struct winlist_entry *w);
void win_set_border(struct winlist_entry *w, win_border_t border);
int win_set_render_mode(struct winlist_entry *w, xc_render_mode_t rmode);
bool win_handle_input(struct winlist_entry *w, int key);
void win_non_focusable(struct winlist_entry *w);
void win_resize(struct winlist_entry *w, int height, int width);
void win_move(struct winlist_entry *w, int y, int x);


#endif
