#include <xc.h>
#include <xc_win.h>
#include <xc_command.h>
#include <xc_render_cmdline.h>

#include <string.h>


char command_buffer[COMMAND_MAX];


int xc_render_cmdline_init(void **ctx)
{
        (void)ctx;
        memset(command_buffer, 0, sizeof(command_buffer));
        return 0;
}


bool xc_render_cmdline_input_cb(void *winlist_e, int key)
{
        /* This window never has context */
        (void)winlist_e;
        (void)key;
        return false;
}


int xc_render_cmdline(void *winlist_e, int w, int h)
{
        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;

        wattron(wle->w->win, COLOR_PAIR(8));
        mvwaddstr(wle->w->win, 0, 0, command_buffer);
        wattroff(wle->w->win, COLOR_PAIR(8));

        return 0;
}


void xc_render_cmdline_release(void **ctx)
{
        (void)ctx;
}
