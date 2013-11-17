#include "protect.h"
#include "keyboard.h"
#include "tty.h"
#include "extern.h"
#include "process.h"

extern int disp_pos;

extern void
init_keyboard();
extern void
select_console(uint8 console_id);

// Global variables
Tty tty_list[CONSOLE_NUM];
Console console_list[CONSOLE_NUM];
uint8 cur_console;

// Private functions
static void
init_tty(Tty *tty);
static void
tty_do_read(Tty *tty);
static void
tty_do_write(Tty *tty);
static void
put_key(Tty *tty, uint32 key);

void
task_tty()
{
    Tty *tty;

    init_keyboard();

    for (tty = tty_list; tty < tty_list + CONSOLE_NUM; ++tty) {
        init_tty(tty);
    }
    select_console(0);
    while (true) {
        for (tty = tty_list; tty < tty_list + CONSOLE_NUM; ++tty) {
            tty_do_read(tty);
            tty_do_write(tty);
        }
    }
}

void
in_process(Tty *tty, uint32 key)
{
    char output[2] = {'\0', '\0'};

    if (!(key & FLAG_EXT) && tty->size < TTY_BUF_SIZE) {
        *(tty->buf_head) = key;
        ++tty->buf_head;
        if (tty->buf_head == tty->buf + TTY_BUF_SIZE) {
            tty->buf_head = tty->buf;
        }
        ++tty->size;
    } else {
        int raw_code = key & MASK_RAW;
        switch (raw_code) {
        case ENTER:
            put_key(tty, '\n');
            break;
        case BACKSPACE:
            put_key(tty, '\b');
            break;
        case UP:
            if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
                scroll_screen(tty->console, SCR_DOWN);
            }
            break;
        case DOWN:
            if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
                scroll_screen(tty->console, SCR_UP);
            }
            break;
        case F1:
        case F2:
        case F3:
        case F4:
        case F5:
        case F6:
        case F7:
        case F8:
        case F9:
        case F10:
        case F11:
        case F12:
            if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R)) {
                select_console(raw_code - F1);
            }
            break;
        default:
            break;
        }
    }
}

int
sys_write(char *buf, int len, Process *proc)
{
    Tty *tty = &tty_list[proc->tty];
    char *p = buf;

    while (len) {
        out_char(tty->console, *p);
        ++p;
        --len;
    }
    return 0;
}


static void
init_tty(Tty *tty)
{
    tty->size = 0;
    tty->buf_head = tty->buf_tail = tty->buf;
    init_screen(tty);
}

static void
tty_do_read(Tty *tty)
{
    if (is_current_console(tty->console)) {
        keyboard_read(tty);
    }
}

static void
tty_do_write(Tty *tty)
{
    if (tty->size > 0) {
        char c = *(tty->buf_tail);
        ++tty->buf_tail;
        if (tty->buf_tail == tty->buf + TTY_BUF_SIZE) {
            tty->buf_tail = tty->buf;
        }
        --tty->size;
        out_char(tty->console, c);
    }
}

static void
put_key(Tty *tty, uint32 key)
{
    if (tty->size < TTY_BUF_SIZE) {
        *(tty->buf_head) = key;
        ++tty->buf_head;
        if (tty->buf_head == tty->buf + TTY_BUF_SIZE) {
            tty->buf_head = tty->buf;
        }
        ++tty->size;
    }
}

