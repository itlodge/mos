#include "type.h"
#include "extern.h"

static int
vsprintf(char *buf, const char *format, va_list args);

int
printf(const char *format, ...)
{
    va_list args = (va_list)((char *)(&format) + 4);
    char buf[256];
    int i = vsprintf(buf, format, args);
    write(buf, i);

    return i;
}

static int
vsprintf(char *buf, const char *format, va_list args)
{
    va_list next_arg = args;
    char tmp[256];
    char *p;
    
    for (p = buf; *format; ++format) {
        if (*format != '%') {
            *p++ = *format;
            continue;
        }
        ++format;
        switch (*format) {
        case 'x':
            uitoa(tmp, *((int *)next_arg));
            strcpy(p, tmp);
            next_arg += 4;
            p += strlen(tmp);
            break;
        case 's':
            break;
        default:
            break;
        }
    }

    return p - buf;
}

