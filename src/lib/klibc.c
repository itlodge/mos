
extern void
disp_str(const char *str);

/**
 * convert a 32-bit unsinged integer N into hex string STR,
 * and return the string.
 */
static char *
uitoa(char *str, int n)
{
    char *p = str;

    if (p == 0) {
        return p;
    }
    *p++ = '0';
    *p++ = 'x';

    if (n == 0) {
        *p = '0';
    }
    for (int i = 28; i >= 0; i -= 4) {
        char c = (n >> i) & 0xFF;
        if (c > 9) {
            c = 'A' + 10 - c;
        } else {
            c += '0';
        }
        *p++ = c;
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

