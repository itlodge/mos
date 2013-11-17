#include "tty.h"
#include "extern.h"
#include "protect.h"

extern Console console_list[CONSOLE_NUM];
extern Tty tty_list[CONSOLE_NUM];
extern uint8 cur_console;

static void
set_cursor(uint32 pos);

static void
set_video_start_addr(uint32 addr);

static void
flush(Console *console);

void
init_screen(Tty *tty)
{
    int tty_num = tty - tty_list;
    tty->console = console_list + tty_num;
    int con_mem_size = (VIDEO_SIZE >> 1) / CONSOLE_NUM;

    tty->console->video_addr = tty_num * con_mem_size;
    tty->console->video_limit = con_mem_size;
    tty->console->cur_start_addr = tty->console->video_addr;
    tty->console->cursor_pos = tty->console->video_addr;

    if (tty_num == 0) {
        tty->console->cursor_pos = disp_pos / 2;
        disp_pos = 0;
    } else {
        out_char(tty->console, tty_num + '0');
        out_char(tty->console, '#');
    }
    set_cursor(tty->console->cursor_pos);
}
   
bool
is_current_console(Console *console)
{
    return console == &(console_list[cur_console]);
}

void
out_char(Console *console, char c)
{
    uint8 *mem = (uint8 *)(VIDEO_BASE + console->cursor_pos * 2);

    switch (c) {
    case '\n':
        if (console->cursor_pos < console->video_addr +
            console->video_limit - SCREEN_WIDTH) {
            console->cursor_pos = console->video_addr + SCREEN_WIDTH *
                ((console->cursor_pos - console->video_addr) /
                 SCREEN_WIDTH + 1);
        }
        break;
    case '\b':
        if (console->cursor_pos > console->video_addr) {
            --console->cursor_pos;
            *(mem - 2) = ' ';
            *(mem - 1) = DEFAULT_CHAR_COLOR;
        }
        break;
    default:
        if (console->cursor_pos < console->video_addr +
            console->video_limit - 1) {
            *mem++ = c;
            *mem++ = DEFAULT_CHAR_COLOR;
            ++console->cursor_pos;
        }
        break;
    }
    while (console->cursor_pos >= console->cur_start_addr + SCREEN_SIZE) {
        scroll_screen(console, SCR_DOWN);
    }
    flush(console);
}

void
select_console(uint8 console_id)
{
    if ((console_id < 0) || (console_id >= CONSOLE_NUM)) {
        return;
    }
    cur_console = console_id;
    flush(&console_list[cur_console]);
}

void
scroll_screen(Console *console, int dir)
{
    if (dir == SCR_UP) {
        if (console->cur_start_addr > console->video_addr) {
            console->cur_start_addr -= SCREEN_WIDTH;
        }
    } else if (dir == SCR_DOWN) {
        if (console->cur_start_addr + SCREEN_SIZE <
            console->video_addr + console->video_limit) {
            console->cur_start_addr += SCREEN_WIDTH;
        }
    }
    flush(console);
}

static void
set_cursor(uint32 pos)
{
    disable_int();
    out_byte(CRTC_ADDR_REG, CURSOR_H);
    out_byte(CRTC_DATA_REG, (pos >> 8) & 0xFF);
    out_byte(CRTC_ADDR_REG, CURSOR_L);
    out_byte(CRTC_DATA_REG, pos & 0xFF);
    enable_int();
}    

static void
set_video_start_addr(uint32 addr)
{
    disable_int();
    out_byte(CRTC_ADDR_REG, START_ADDR_H);
    out_byte(CRTC_DATA_REG, (addr >> 8) & 0xFF);
    out_byte(CRTC_ADDR_REG, START_ADDR_L);
    out_byte(CRTC_DATA_REG, addr & 0xFF);
    enable_int();
}

static void
flush(Console *console)
{
    if (is_current_console(console)) {
        set_cursor(console->cursor_pos);
        set_video_start_addr(console->cur_start_addr);
    }
}
