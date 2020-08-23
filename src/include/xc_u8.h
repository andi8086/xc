#ifndef __XC_U8_H__
#define __XC_U8_H__

#define _XOPEN_SOURCE

#include <wchar.h>
#include <string.h>
#include <stdlib.h>

int mbswidth(char *string);
int strncpy_u8(char *dest, char *src, size_t len);
char *strabbrev_u8(char *str, int maxwidth);


#endif
