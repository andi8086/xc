#include <xc_win.h>
#include <xc_render_output.h>
#include <xc_viewport.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>


void xc_output_set(void *winlist_e, char *out, char *err, void *parent)
{
        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;
        struct xc_render_output_ctx *ctx =
                (struct xc_render_output_ctx *)wle->w->render_ctx;
        ctx->stdout_buffer = out;
        ctx->stderr_buffer = err;
        ctx->old_focus = parent;
}


int xc_render_output_init(void **ctx)
{
        xcr_output_t **pctx = (xcr_output_t **)ctx;

        /* Create a shared memory mapping for forked child process */
        *pctx = malloc(sizeof(xcr_output_t));

        if (!*pctx) {
                return -ENOMEM;
        }

        (*pctx)->stdout_buffer = NULL;
        (*pctx)->stderr_buffer = NULL;
        (*pctx)->old_focus = NULL;
        (*pctx)->xpos = 0;
        (*pctx)->ypos = 0;
        (*pctx)->buffsel = 0;
        return 0;
}


bool xc_render_output_input_cb(void *winlist_e, int key)
{
        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;
        struct xc_render_output_ctx *ctx =
                (struct xc_render_output_ctx *)wle->w->render_ctx;

        switch (key) {
        case 0x1B:
        case 'q':
                win_set_focus(ctx->old_focus);
                win_destroy(winlist_e);
                win_redraw_list(true);
                return true;
        case KEY_DOWN:
        case 'j':
                ctx->ypos++;
                return true;
        case KEY_UP:
        case 'k':
                if (ctx->ypos == 0) {
                        return true;
                }
                ctx->ypos--;
                return true;
        case KEY_RIGHT:
        case 'l':
                ctx->xpos++;
                return true;
        case KEY_LEFT:
        case 'h':
                if (ctx->xpos == 0) {
                        return true;
                }
                ctx->xpos--;
                return true;
        case 'e':
                ctx->buffsel = 1;
                return true;
        case 'o':
                ctx->buffsel = 0;
                return true;
        }

        return false;
}


int xc_render_output(void *winlist_e, int w, int h)
{
        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;
        struct xc_render_output_ctx *ctx =
                (struct xc_render_output_ctx *)wle->w->render_ctx;

        switch (ctx->buffsel) {
        case 0:
                if (!ctx->stdout_buffer) {
                        break;
                }
                xc_viewport_display(winlist_e, ctx->stdout_buffer, 1, 1,
                                    w, h-1, ctx->xpos, ctx->ypos);
                break;
        case 1:
                if (!ctx->stderr_buffer) {
                        break;
                }
                xc_viewport_display(winlist_e, ctx->stderr_buffer, 1, 1,
                                    w, h-1, ctx->xpos, ctx->ypos);
                break;
        default:
                break;
        }
        wattron(wle->w->win, COLOR_PAIR(9));
        mvwaddstr(wle->w->win, h, 1,
                  "h/j/k/l navigate, q(uit), std(o)ut, std(e)rr");
        wattroff(wle->w->win, COLOR_PAIR(3));
        return 0;
}


void xc_render_output_release(void **ctx)
{
        xcr_output_t *pctx = *(xcr_output_t **)ctx;
        if (pctx->stdout_buffer) {
                free(pctx->stdout_buffer);
        }
        if (pctx->stderr_buffer) {
                free(pctx->stderr_buffer);
        }
        free(*ctx);
        *ctx = NULL;
}
