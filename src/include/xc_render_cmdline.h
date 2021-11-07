#ifndef XC_RENDER_CMDLINE_H
#define XC_RENDER_CMDLINE_H

#include <xc_render.h>

int xc_render_cmdline(void *inlist_e, int w, int h);
int xc_render_cmdline_init(void **ctx);
void xc_render_cmdline_release(void **ctx);
bool xc_render_cmdline_input_cb(void *winlist_e, int key);

#endif
