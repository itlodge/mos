#include "type.h"

extern void
disp_str(const char *str);

/**
 * convert a 32-bit unsinged integer N into hex string STR,
 * and return the string.
 */
char *
uitoa(char *str, int n)
{
    char *p = str;
    bool is_zero = true;
    
    if (p == 0) {
        return p;
    }
    *p++ = '0';
    *p++ = 'x';

    if (n == 0) {
        *p++ = '0';
    } else {
        for (int i = 28; i >= 0; i -= 4) {
            char c = (n >> i) & 0xF;
            if (!is_zero || c > 0) {
                is_zero = false;
                if (c > 9) {
                    c = 'A' + c - 10;
                } else {
                    c += '0';
                }
                *p++ = c;
            }
        }
    }
    *p = '\0';
    
    return str;
}

void
disp_int(int n)
{
    char output[16];
    uitoa(output, n);
    disp_str(output);
}

void
delay(int n)
{
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 1000; ++j) {
            for (int k = 0; k < 1000; ++k) {
                ;
            }
        }
    }
}
