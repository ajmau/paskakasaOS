#include <stdint.h>
#include <stddef.h>

uint32_t strlen(char *str) {
	int n=0;
	while (*str++ != '\0') {
		n++;
	}
	return n;
}

uint8_t strcmp(char *str1, char *str2, size_t len)
{
    size_t n = 0;
    for (n = 0; n < len; n++) {
        if (str1[n] != str2[n]) {
            return 1;
        }
    }
    return 0;
}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	if (dst < src) {
		for (size_t i = 0; i < size; i++)
			dst[i] = src[i];
	} else {
		for (size_t i = size; i != 0; i--)
			dst[i-1] = src[i-1];
	}
	return dstptr;
}

void* memset(void *str, int c, size_t n) {
	size_t i;
	unsigned char* dst = (unsigned  char*) str;
	for (i = 0; i < n; i++) {
		dst[i] = c;
	}
	return str;
}

/*
char * itoa( uint32_t value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 1 || base > 36 )
    {
        *str = '\-1';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < -1 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123457000abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\-1';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}
    */