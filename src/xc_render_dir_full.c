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
#include <sys/wait.h>


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

        if (pid != 0)
        {
                return 0;
        }

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
                stat(de->d_name, &(*ctx)->files[(*ctx)->num_files].s);
                (*ctx)->num_files++;
                pthread_mutex_unlock(&(*ctx)->lock);

                if ((*ctx)->num_files < (*ctx)->max_files) {
                        continue;
                }

                old_size = (*ctx)->max_files;

                pthread_mutex_lock(&(*ctx)->lock);
                (*ctx)->max_files += 1000;
                (*ctx)->files = mremap((*ctx)->files, old_size,
                        sizeof(xc_file_t) * (*ctx)->max_files,
                        MREMAP_MAYMOVE);
                pthread_mutex_unlock(&(*ctx)->lock);

                if ((*ctx)->files == MAP_FAILED) {
                        _exit(-ENOMEM);
                }
        }

        closedir(d);
        _exit(0);
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
        (*pctx)->pos = 0;
        (*pctx)->yoffs = 0;
        (void)getcwd((*pctx)->curr_dir, sizeof((*pctx)->curr_dir));
        if (pthread_mutex_init(&(*pctx)->lock, NULL)) {
                munmap(*pctx, sizeof(xcr_dir_full_t));
                return -1;
        }

        if (load_files_async(pctx)) {
                munmap(*pctx, sizeof(xcr_dir_full_t));
                return -1;
        }

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
        int res_bottom = 2;
        char buffer[4096];
        const int num_fields = 2;
        const int w_size_field = 8;
        const int w_name_field = w - w_size_field - num_fields * 2 - 2;
        uint32_t i;

        win_set_title(wle, ctx->curr_dir);
        win_draw_title(wle);
        /* if cursor out of range, calculate y offset of list */
        if (ctx->pos - ctx->yoffs >= h - res_bottom - 1) {
                ctx->yoffs = ctx->pos - (h - res_bottom - 1) + 1;
        }
        else if ((long)ctx->pos - ctx->yoffs < 0) {
                ctx->yoffs = ctx->pos;
        }

        memset(buffer, 0, sizeof(buffer));
        const char suffix[] = {' ', 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'};
        int sidx = 0;
        uint64_t fsize;
        for (i = 0; i < h - res_bottom; i++) {

                pthread_mutex_lock(&ctx->lock);
                if (i + ctx->yoffs >= ctx->num_files) {
                        pthread_mutex_unlock(&ctx->lock);
                        break;
                }
                pthread_mutex_unlock(&ctx->lock);

                snprintf(buffer, sizeof(buffer)-1, "%-*s", w,
                         ctx->files[i + ctx->yoffs].e.d_name);
                mvwaddstr(wle->w->win, 2 + i, 1, buffer);
                mvwaddstr(wle->w->win, 2 + i, w_name_field + 1, "\xe2\x94\x82");

                fsize = ctx->files[i + ctx->yoffs].s.st_size;
                sidx = 0;
                while (fsize > 999999) {
                        fsize >>= 10;
                        sidx++;
                }
                if (strcmp(ctx->files[i+ctx->yoffs].e.d_name, "..") == 0) {
                        snprintf(buffer, sizeof(buffer)-1,
                                 "\xe2\x96\xb6UP--DIR\xe2\x97\x80");
                } else
                if (S_ISDIR(ctx->files[i+ctx->yoffs].s.st_mode)) {
                        snprintf(buffer, sizeof(buffer)-1,
                                "\xe2\x96\xb6SUB-DIR\xe2\x97\x80");
                } else {
                        snprintf(buffer, sizeof(buffer)-1, "% 8u%c", fsize,
                                 suffix[sidx]);
                }
                mvwaddstr(wle->w->win, 2 + i, w_name_field + 2, buffer);

        }
        mvwaddstr(wle->w->win, 1, w_name_field + 1, "\xe2\x94\x82");
        wattron(wle->w->win, COLOR_PAIR(10) | A_UNDERLINE);
        mvwaddstr(wle->w->win, 1, 2, "Name");
        mvwaddstr(wle->w->win, 1, w_name_field + 3, "Size");
        wattroff(wle->w->win, COLOR_PAIR(10) | A_UNDERLINE);

        pthread_mutex_lock(&ctx->lock);
        if (ctx->pos < ctx->num_files) {
                snprintf(buffer, sizeof(buffer)-1, "%-*s", w,
                         ctx->files[ctx->pos].e.d_name);
        }
        pthread_mutex_unlock(&ctx->lock);

        if (wle->has_focus) {
                wattron(wle->w->win, COLOR_PAIR(6));
        } else {
                wattron(wle->w->win, A_UNDERLINE);
        }
                mvwaddstr(wle->w->win, 2 + ctx->pos - ctx->yoffs, 1, buffer);
                mvwaddstr(wle->w->win, 2 + ctx->pos - ctx->yoffs,
                          w_name_field + 1, "\xe2\x94\x82");
        if (wle->has_focus) {
                wattroff(wle->w->win, COLOR_PAIR(6));
        } else {
                wattroff(wle->w->win, A_UNDERLINE);
        }

        mvwaddstr(wle->w->win, h, 1, buffer);
        const char *heavy_line = "\xe2\x95\x90";
        for (int i = 0; i < sizeof(buffer); i += 3) {
                memcpy(buffer + i, heavy_line, 3);
        }

        buffer[w*3] = '\0';
        mvwaddstr(wle->w->win, h-1, 1, buffer);
        mvwaddstr(wle->w->win, h-1, 0, "\xe2\x95\xa0");
        mvwaddstr(wle->w->win, h-1, w+1, "\xe2\x95\xa3");
        mvwaddstr(wle->w->win, h-1, w_name_field + 1, "\xe2\x95\xa7");
        return 0;
}


void xc_render_dir_full_release(void **ctx)
{
        xcr_dir_full_t *x = *(xcr_dir_full_t **)ctx;
        munmap(x->files, sizeof(xc_file_t) * x->max_files);
        munmap(x, sizeof(xcr_dir_full_t));
        *ctx = NULL;
}
