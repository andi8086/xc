#include <xc_u8.h>


int mbswidth(char *string)
{
	wchar_t buff[5];
	if (!string) {
		return -1;
	}
	
	int len;
	int width = 0;
	while (*string) {
		len = mbtowc(buff, string, 5);
		string += len;
		width += wcwidth(buff[0]);
	}
	return width;
}


int strncpy_u8(char *dest, char *src, size_t len)
{
	if (!src || !dest || len < 2) {
		return 0;
	}
	memcpy(dest, src, len);
	dest[len-1] = 0;
	/* in a multibyte sequence, the 2nd till 4th
	 * byte always begin with binary 10xx ...
	 * Then we only have to subtract
	 * one more for Byte 1, which always starts with
	 * 11xx ... */
	len--;
	if ((dest[len-1] & 0xC0) == 0xC0) {
		dest[len-1] = 0;
		return len-1;
	}
	while(len && ((dest[len-1] & 0xC0) == 0x80)) {
		len--;
	};
	dest[len-1] = 0;
	return len-1;
}


char *strabbrev_u8(char *str, int maxwidth)
{
	if (!str) {
		return NULL;
	}
	
	int l = mbswidth(str);
	int b = strlen(str);
	char *newstr = malloc(b);
	if (!newstr) {
		return NULL;
	}
	if (l <= maxwidth) {
		return strcpy(newstr, str);
	}
	
	do {
		strncpy_u8(newstr, str, maxwidth);
	} while(mbswidth(newstr) > (maxwidth--) && maxwidth);

	return newstr;
}

