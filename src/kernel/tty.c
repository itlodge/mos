#include "keyboard.h"

extern void
disp_str(const char *str);

void
task_tty()
{
    while (1) {
        keyboard_read();
    }
}

void
in_process(uint32 key)
{
    char output[2] = {'\0', '\0'};

    if (!(key & FLAG_EXT)) {
        output[0] = key & 0xFF;
        disp_str(output);
    }
}
