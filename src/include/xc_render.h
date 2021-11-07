#ifndef XC_RENDER_H
#define XC_RENDER_H

#include <stdbool.h>

/* call back function to draw window content */
typedef int (*xc_render_func_t)(void *inlist_e, int w, int h);
typedef int (*xc_render_init_t)(void **ctx);
typedef void (*xc_render_release_t)(void **ctx);
typedef bool (*xc_render_input_cb_t)(void *winlist_e, int key);

typedef struct xc_render_info {
        xc_render_func_t render_draw;
        xc_render_init_t render_init;
        xc_render_release_t render_release;
        xc_render_input_cb_t render_input_cb;
} xc_render_info_t;

#include <xc_render_dir_full.h>
#include <xc_render_keys.h>
#include <xc_render_cmdline.h>

typedef enum {
        RENDERMODE_DIR_FULL = 0,
        RENDERMODE_KEYS,
        RENDERMODE_CMDLINE
} xc_render_mode_t;

extern xc_render_info_t render_infos[3];

void xc_render_init(void);

#endif
