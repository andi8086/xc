#ifndef XC_RENDER_KEYS_H
#define XC_RENDER_KEYS_H

#include <xc_render.h>
#include <xc_files.h>
#include <limits.h>
#include <stdint.h>
#include <pthread.h>

typedef struct xc_render_keys_ctx {
        char names[12][8];
} xcr_keys_t;


int xc_render_keys(void *inlist_e, int w, int h);
int xc_render_keys_init(void **ctx);
void xc_render_keys_release(void **ctx);
bool xc_render_keys_input_cb(void *winlist_e, int key);

#endif

