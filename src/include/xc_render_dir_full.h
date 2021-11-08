#ifndef XC_RENDER_DIR_FULL_H
#define XC_RENDER_DIR_FULL_H

#include <xc_render.h>
#include <xc_files.h>
#include <limits.h>
#include <stdint.h>
#include <pthread.h>

typedef struct xc_render_dir_full_ctx {
        int pos;
        uint32_t yoffs;
        char curr_dir[PATH_MAX];
        xc_file_t *files; 
        uint32_t num_files;
        uint32_t max_files;
        pthread_mutex_t lock;
} xcr_dir_full_t;


int xc_render_dir_full(void *inlist_e, int w, int h);
int xc_render_dir_full_init(void **ctx);
void xc_render_dir_full_release(void **ctx);
bool xc_render_dir_full_input_cb(void *winlist_e, int key);

#endif

