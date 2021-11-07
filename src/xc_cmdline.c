#include <xc.h>
#include <xc_u8.h>
#include <xc_win.h>
#include <xc_command.h>
#include <xc_render_cmdline.h>
#include <xc_async.h>

#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

char command_buffer[COMMAND_MAX];
int command_cursor;
int command_cursor_offset;

int xc_render_cmdline_init(void **ctx)
{
        (void)ctx;
        memset(command_buffer, 0, sizeof(command_buffer));
        command_cursor = 0;
        return 0;
}


int xc_execute_command(char *cmd, char **out_buffer, char **err_buffer)
{
        int res;
        int out_buffer_chunk = 10240;
        int err_buffer_chunk = 10240;
        int out_chunks = 1;
        int err_chunks = 1;
        char out_chunk[out_buffer_chunk];
        char err_chunk[err_buffer_chunk];

        (*out_buffer) = malloc(out_buffer_chunk);
        (*err_buffer) = malloc(err_buffer_chunk);

        char *argv[4] = {
                "bash",
                "-c",
                cmd,
                NULL
        };
        async_info_t async;

        res = async_exec(&async, "/bin/bash", argv);

        if (res) {
                return res;
        }

        while (1) {
                size_t count = read(async.std_out, out_chunk, sizeof(out_chunk));
                if (count == -1) {
                        if (errno = EINTR) {
                                continue;
                        } else {
                                break;
                        }
                } else if (count == 0) {
                        break;
                } else {
                        /* we read a whole chunk, store it into the buffer */
                        /* and grow the buffer */
                        memcpy(*out_buffer, out_chunk, sizeof(out_chunk));
                        out_chunks++;
                        *out_buffer = realloc(*out_buffer,
                                              out_chunks * sizeof(out_chunk));
                        if (!*out_buffer) {
                                break;
                        }
                        memset(*out_buffer + (sizeof(out_chunk) *
                                             (out_chunks - 1)),
                               0, sizeof(out_chunk));
                }
        }

        while (1) {
                size_t count = read(async.err_out, err_chunk,
                                    sizeof(err_chunk));
                if (count == -1) {
                        if (errno = EINTR) {
                                continue;
                        } else {
                                break;
                        }
                } else if (count == 0) {
                        break;
                } else {
                        /* we read a whole chunk, store it into the buffer */
                        /* and grow the buffer */
                        memcpy(*err_buffer, err_chunk, sizeof(err_chunk));
                        err_chunks++;
                        *err_buffer = realloc(*err_buffer,
                                              err_chunks * sizeof(err_chunk));
                        if (!*err_buffer) {
                                break;
                        }
                        memset(*err_buffer + (sizeof(err_chunk) *
                                             (err_chunks - 1)),
                               0, sizeof(err_chunk));
                }
        }

        close(async.err_out);
        close(async.std_out);
        wait(0);
}


static void clear_command(void)
{
        command_cursor = 0;
        command_cursor_offset = 0;
        memset(command_buffer, 0, sizeof(command_buffer));
}


bool xc_render_cmdline_input_cb(void *winlist_e, int key)
{
        char *out_buff, *err_buff;
        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;
        struct winlist_entry *output_win;
        wrefresh(wle->w->win);

        /* This window never has context */
        (void)winlist_e;

        if (key >= 0x1C && key <= 0x7E) {
                command_buffer[command_cursor] = (char)key;
                command_cursor++;
                goto key_handled;
        }

        switch (key) {
        case KEY_DL:
                break;
        case KEY_BACKSPACE:
                if (command_cursor > 0) {
                        command_cursor--;
                        command_buffer[command_cursor] = '\0';
                }
                goto key_handled;
        case '\n':
                /* execute command */
                if (strlen(command_buffer)) {
                        xc_execute_command(command_buffer,
                                           &out_buff, &err_buff);
                        output_win = win_create_c(LINES-10, COLS-20, 5, 10, 8);
                        win_set_render_mode(output_win, RENDERMODE_OUTPUT);
                        win_set_title(output_win, "Cmd output");
                        xc_output_set(output_win, out_buff, err_buff, win_get_focused());
                        win_set_focus(output_win);
                        clear_command();
                        goto key_handled;
                }
                break;
        default:
                break;
        }

key_not_handled:
        return false;
key_handled:
        if (command_cursor > COLS - 2) {
                command_cursor_offset = command_cursor - (COLS - 2);
        } else {
                command_cursor_offset = 0;
        }
        return true;
}


int xc_render_cmdline(void *winlist_e, int w, int h)
{
        extern bool cursor;
        extern uint64_t timer_counter;
        uint64_t tc;

        __atomic_load(&timer_counter, &tc, __ATOMIC_CONSUME);

        struct winlist_entry *wle = (struct winlist_entry *)winlist_e;
        wrefresh(wle->w->win);
        werase(wle->w->win);

        char curr_path[PATH_MAX];
        memset(curr_path, 0, sizeof(curr_path));
        getcwd(curr_path, PATH_MAX - 1);

        /* show current path */
        char abbrev_path[PATH_MAX];
        (void)strabbrev_s_u8(curr_path, abbrev_path, COLS-2);
        wattron(wle->w->win, COLOR_PAIR(3));
        mvwaddstr(wle->w->win, 0, 0, abbrev_path);
        wattroff(wle->w->win, COLOR_PAIR(3));

        wattron(wle->w->win, COLOR_PAIR(8));
        mvwaddstr(wle->w->win, 1, 0, ">");
        mvwaddstr(wle->w->win, 1, 1, &command_buffer[command_cursor_offset]);
        if (tc & 4) {
                mvwaddstr(wle->w->win, 1, command_cursor - command_cursor_offset
                                          + 1, "|");
        } else {
                mvwaddstr(wle->w->win, 1, command_cursor - command_cursor_offset
                                          + 1, " ");
        }

//        mvwaddstr(wle->w->win, 1, 2, command_buffer);
        wattroff(wle->w->win, COLOR_PAIR(8));
        return 0;
}


void xc_render_cmdline_release(void **ctx)
{
        (void)ctx;
}
