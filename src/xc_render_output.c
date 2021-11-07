#include <xc_win.h>
#include <xc_render_output.h>

#include <stdlib.h>
#include <errno.h>


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
        return 0;
}


bool xc_render_output_input_cb(void *winlist_e, int key)
{
        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;
        struct xc_render_output_ctx *ctx =
                (struct xc_render_output_ctx *)wle->w->render_ctx;
        /* This window never has context */
        (void)winlist_e;
        (void)key;

        switch (key) {
        case 0x1B:
                win_set_focus(ctx->old_focus);
                win_draw(ctx->old_focus);
                win_destroy(winlist_e);
                //wclear(stdscr);
                win_redraw_list(true);
                return true;
        }

        return false;
}


int xc_render_output(void *winlist_e, int w, int h)
{
        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;
        struct xc_render_output_ctx *ctx =
                (struct xc_render_output_ctx *)wle->w->render_ctx;

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
