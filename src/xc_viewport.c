#include <xc_win.h>
#include <xc_viewport.h>

#include <stdlib.h>
#include <malloc.h>
#include <string.h>


void xc_viewport_display(void *winlist_e, char *buffer, int x, int y,
                         int w, int h, int xpos, int ypos)
{
        h++;
        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;
        struct xc_render_output_ctx *ctx =
                (struct xc_render_output_ctx *)wle->w->render_ctx;

        char clearline[4096];
        char *displine;
        const char delim[2] = "\n";
        char *line;
        size_t stdout_size = malloc_usable_size(buffer);
        char *stdout2 = malloc(stdout_size);
        if (!stdout2) {
                return;
        }
        memcpy(stdout2, buffer, stdout_size);
        line = strtok(stdout2, delim);
        snprintf(clearline, 4095, "%-*s", w, " ");

        int i;
        for (i = 0; i < h; i++) {
                mvwaddstr(wle->w->win, i + y, x, clearline);
        }

        /* strtok did not find a line ending, so print one line only */
        if (!line) {
                if (strlen(stdout2) && xpos < strlen(stdout2)) {
                        mvwaddstr(wle->w->win, y, x, &stdout2[xpos]);
                }
                free(stdout2);
                return;
        }

        i = 0;
        do {
                if (((i - ypos) >= h - 1) ||
                        (i < ypos)) {
                        i++;
                        continue;
                }
                if (xpos >= strlen(line)) {
                        i++;
                        continue;
                }
                if (strlen(&line[xpos]) >= w) {
                        line[xpos + w] = '\0';
                }
                if (strlen(&line[xpos])) {
                        mvwaddstr(wle->w->win, i - ypos + y, x,
                                  &line[xpos]);
                }
                i++;
        } while (line = strtok(NULL, delim));
        free(stdout2);
}


