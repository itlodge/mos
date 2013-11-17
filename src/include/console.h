#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "type.h"

#define VIDEO_BASE 0xB8000
#define VIDEO_SIZE 0x8000

#define SCR_UP 1
#define SCR_DOWN -1

struct Tty;

typedef struct Console {
    uint32 cur_start_addr;
    uint32 video_addr;
    uint32 video_limit;
    uint32 cursor_pos;
} Console;

// Public functions
bool
is_current_console(Console *console);

void
out_char(Console *console, char c);

void
init_screen(struct Tty *tty);

void
scroll_screen(Console *console, int dir);

#endif /* _CONSOLE_H_ */
