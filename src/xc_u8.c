#include <xc_u8.h>


int mbswidth(char *string)
{
	size_t n = mbstowcs(NULL, string, 0) + 1;
	wchar_t *wcstring = malloc(n * sizeof(wchar_t));
	if (!wcstring) {
		return -1;
	}
	if (mbstowcs(wcstring, string, n) == -1) {
		free(wcstring);
		return -1;
	}
	int width = wcswidth(wcstring, n);
	free(wcstring);
	return width;
}


int strncpy_u8(char *dest, char *src, size_t len)
{
	if (!src || !dest || !len) {
		return 0;
	}

	/* in a multibyte sequence, the 2nd till 4th
	 * byte always begin with binary 10xx ...
	 * Then we only have to subtract
	 * one more for Byte 1, which always starts with
	 * 11xx ... */
	size_t j = len - 1;
	while(j && (src[j] & 0xC0 == 0x80)) {
		j--;
	};
	j--;
	strncpy(dest, src, j);

	/* we expect to have one byte more */
	dest[j] = 0;
	return j;
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

	printf("%d\n", mbswidth(newstr));
	return newstr;
}

