#define _GNU_SOURCE
#include <xc.h>
#include <xc_win.h>
#include <xc_render_keys.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


int xc_render_keys_init(void **ctx)
{
        xcr_keys_t **pctx = (xcr_keys_t **)ctx;

        /* Create a shared memory mapping for forked child process */
        *pctx = malloc(sizeof(xcr_keys_t));

        if (!*pctx) {
                return -ENOMEM;
        }

        const char *func_names[12] = {
                "Help", "UsrMen", "View",
                "Edit", "Copy", "Move",
                "MkDir", "Delete", "Menu",
                "Quit", "CloseWin", "NewWin"
        };

        for (int i = 0; i < 12; i++) {
                memset((*pctx)->names[i], 0, 12);
                snprintf((*pctx)->names[i], 11, "%s", func_names[i]);
        }

        return 0;
}


bool xc_render_keys_input_cb(void *winlist_e, int key)
{
        /* This window never has context */
        (void)winlist_e;
        (void)key;
        return false;
}


int xc_render_keys(void *winlist_e, int w, int h)
{
        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;
        struct xc_render_keys_ctx *ctx =
                (struct xc_render_keys_ctx *)wle->w->render_ctx;

        char buffer[12];
        int key_width = wle->w->w / 12 - 2;
        int key_offset;

        for (int i = 0; i < 12; i++) {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "%d", i + 1);
                wattron(wle->w->win, COLOR_PAIR(8));
                mvwaddstr(wle->w->win, 0, wle->w->w / 12 * i, buffer);
                wattroff(wle->w->win, COLOR_PAIR(8));
                snprintf(buffer, sizeof(buffer)-1, "%-*s",
                         key_width, ctx->names[i]);
                wattron(wle->w->win, COLOR_PAIR(6));

                key_offset = 1;
                if (i > 8) {
                        key_offset += 1;
                }
                mvwaddstr(wle->w->win, 0, wle->w->w / 12 * i + key_offset,
                          buffer);
                wattroff(wle->w->win, COLOR_PAIR(6));
        }

        return 0;
}


void xc_render_keys_release(void **ctx)
{
        free(*ctx);
        *ctx = NULL;
}
