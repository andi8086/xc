#ifndef XC_RENDER_OUTPUT_H
#define XC_RENDER_OUTPUT_H

#include <xc_render.h>


typedef struct xc_render_output_ctx {
        char *stdout_buffer;
        char *stderr_buffer;
        void *old_focus;
        int xpos, ypos;
        int buffsel;
} xcr_output_t;


void xc_output_set(void *winlist_e, char *out, char *err, void *parent);
int xc_render_output(void *inlist_e, int w, int h);
int xc_render_output_init(void **ctx);
void xc_render_output_release(void **ctx);
bool xc_render_output_input_cb(void *winlist_e, int key);

#endif
