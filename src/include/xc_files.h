#ifndef XC_FILES_H
#define XC_FILES_H

#include <dirent.h>
#include <sys/stat.h>


typedef struct xc_file {
        struct dirent e;
        struct stat s;
} xc_file_t;


#endif
