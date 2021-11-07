#ifndef XC_ASYNC
#define XC_ASYNC

typedef enum {
        ASYNC_OK,
        ASYNC_PIPE,
        ASYNC_NOFORK,
        ASYNC_EXECV
} async_error_t;


typedef struct async_info {
        int std_out;
        int err_out;
} async_info_t;


int async_exec(async_info_t *async, char *path, char * const argv[]);


#endif
