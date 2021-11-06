#define _GNU_SOURCE
#include <xc.h>
#include <xc_win.h>
#include <xc_render_dir_full.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <xc_files.h>
#include <errno.h>
#include <sys/mman.h>


static int load_files_async(xcr_dir_full_t **ctx)
{
        /* reserve space for 1000 files, remap on demand */
        (*ctx)->num_files = 0;
        (*ctx)->files = mmap(0, sizeof(xc_file_t) * 1000,
                PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (!(*ctx)->files) {
                return -ENOMEM;
        }
        (*ctx)->max_files = 1000;

        pid_t pid = fork();
        if (pid == -1) {
                return -1;
        }

        if (pid == 0) {
                /* forked child process */
                DIR *d = opendir((*ctx)->curr_dir);
                struct dirent *de;
                uint32_t old_size;
                if (!d) {
                        _exit(-1);
                }

                /* we hard-code the '..' as first element */
                pthread_mutex_lock(&(*ctx)->lock);
                sprintf((*ctx)->files[0].e.d_name, "..");
                (*ctx)->num_files++;
                pthread_mutex_unlock(&(*ctx)->lock);

                while (de = readdir(d)) {
                        /* skip . and .. */
                        if (de->d_name[0] == '.' &&
                            de->d_name[1] == '\0') {
                               continue;
                        }
                        if (de->d_name[0] == '.' &&
                            de->d_name[1] == '.' &&
                            de->d_name[2] == '\0') {
                               continue;
                        }

                        pthread_mutex_lock(&(*ctx)->lock);
                        memcpy(&(*ctx)->files[(*ctx)->num_files].e, de,
                               sizeof(struct dirent));
                        (*ctx)->num_files++;
                        pthread_mutex_unlock(&(*ctx)->lock);
                        if ((*ctx)->num_files == (*ctx)->max_files) {
                                old_size = (*ctx)->max_files;

                                pthread_mutex_lock(&(*ctx)->lock);
                                (*ctx)->max_files += 1000;
                                (*ctx)->files = mremap((*ctx)->files, old_size,
                                                       (*ctx)->max_files,
                                                       MREMAP_MAYMOVE);
                                pthread_mutex_unlock(&(*ctx)->lock);
                                if ((*ctx)->files == MAP_FAILED) {
                                        _exit(-ENOMEM);
                                }
                        }
                }

                closedir(d);
                _exit(0);
        }

        return 0;
}


int xc_render_dir_full_init(void **ctx)
{
        xcr_dir_full_t **pctx = (xcr_dir_full_t **)ctx;
        /* Create a shared memory mapping for forked child process */
        *pctx = mmap(NULL, sizeof(xcr_dir_full_t),
                     PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

        if (*pctx == MAP_FAILED) {
                return -ENOMEM;
        }
        (void)getcwd((*pctx)->curr_dir, sizeof((*pctx)->curr_dir));
        (*pctx)->pos = 0;

        if (pthread_mutex_init(&(*pctx)->lock, NULL)) {
                munmap(*pctx, sizeof(xcr_dir_full_t));
                return -1;
        }

        if (load_files_async(pctx)) {
                munmap(*pctx, sizeof(xcr_dir_full_t));
                return -1;
        }

 //     munmap(*pctx, sizeof(xcr_dir_full_t));

        return 0;
}


bool xc_render_dir_full_input_cb(void *winlist_e, int key)
{
        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;
        struct xc_render_dir_full_ctx *ctx =
                (struct xc_render_dir_full_ctx *)wle->w->render_ctx;

        switch (key) {
        case KEY_UP:
                pthread_mutex_lock(&ctx->lock);
                if (ctx->pos > 0) {
                        ctx->pos--;
                }
                pthread_mutex_unlock(&ctx->lock);
                return true;
        case KEY_DOWN:
                pthread_mutex_lock(&ctx->lock);
                if (ctx->num_files && (ctx->pos < ctx->num_files - 1)) {
                        ctx->pos++;
                }
                pthread_mutex_unlock(&ctx->lock);
                return true;
        }

        return false;
}


int xc_render_dir_full(void *winlist_e, int w, int h)
{
        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;
        struct xc_render_dir_full_ctx *ctx =
                (struct xc_render_dir_full_ctx *)wle->w->render_ctx;

        char buffer[4096];
        memset(buffer, 0, sizeof(buffer));

        uint32_t i = 0;
        while (1) {
                pthread_mutex_lock(&ctx->lock);
                if (!ctx || i >= ctx->num_files) {
                        pthread_mutex_unlock(&ctx->lock);
                        break;
                }
                pthread_mutex_unlock(&ctx->lock);
                snprintf(buffer, sizeof(buffer)-1, "%-*s", w,
                         ctx->files[i].e.d_name);
                mvwaddstr(wle->w->win, i+1, 1, buffer);
                i++;
        }

        if (wle->has_focus) {
                wattron(wle->w->win, COLOR_PAIR(6));
        }

        pthread_mutex_lock(&ctx->lock);
        if (ctx->pos < ctx->num_files) {
                snprintf(buffer, sizeof(buffer)-1, "%-*s", w,
                         ctx->files[ctx->pos].e.d_name);
        }
        pthread_mutex_unlock(&ctx->lock);
        mvwaddstr(wle->w->win, ctx->pos+1, 1, buffer);

        if (wle->has_focus) {
                wattroff(wle->w->win, COLOR_PAIR(6));
        }

        return 0;
}


void xc_render_dir_full_release(void **ctx)
{
        free(*ctx);
        *ctx = NULL;
}
