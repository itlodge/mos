#include "keyboard.h"
#include "keymap.h"

// External functions from ASM
extern void
enable_irq(int irq);

extern uint8
in_byte(uint16 port);

extern void
disable_int();

extern void
enable_int();

// External functions from C files
extern void
put_irq_handler(int irq, irq_handler handler);

extern void
in_process(uint32 key);

// Private variables
static KbBuf kb_buf;

static int code_E0;
static int shift_l;
static int shift_r;
static int alt_l;
static int alt_r;
static int ctrl_l;
static int ctrl_r;
static int caps_lock;
static int num_lock;
static int scroll_lock;
static int column;

// Private functions
static uint8
get_code_from_kbuf();

void
keyboard_handler(int irq)
{
    uint8 scan_code = in_byte(KB_DATA);
    if (kb_buf.size < KB_MAX_SIZE) {
        *(kb_buf.head) = scan_code;
        ++kb_buf.head;
        if (kb_buf.head == kb_buf.buf + KB_MAX_SIZE) {
            kb_buf.head = kb_buf.buf;
        }
        ++kb_buf.size;
    }
}

void
init_keyboard()
{
    kb_buf.size = 0;
    kb_buf.head = kb_buf.tail = kb_buf.buf;

    shift_l = shift_r = false;
    alt_l = alt_r = false;
    ctrl_l = ctrl_r = false;

    put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
    enable_irq(KEYBOARD_IRQ);
}

void
keyboard_read()
{
    uint8 scan_code;
    bool is_make;

    uint32 key = 0;
    uint32 *keyrow;
    
    if (kb_buf.size > 0) {
        code_E0 = 0;
        scan_code = get_code_from_kbuf();

        if (scan_code == 0xE1) {
            uint8 pause_code[] = {0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5};
            bool is_pause = true;
            for (int i = 1; i < sizeof(pause_code) / sizeof(pause_code[0]);
                 ++i) {
                if (get_code_from_kbuf() != pause_code[i]) {
                    is_pause = false;
                    break;
                }
            }
            if (is_pause) {
                key = PAUSEBREAK;
            }
        } else if (scan_code == 0xE0) {
            scan_code = get_code_from_kbuf();
            if (scan_code == 0x2A) {
                if (get_code_from_kbuf() == 0xE0) {
                    if (get_code_from_kbuf() == 0x37) {
                        key = PRINTSCREEN;
                        is_make = true;
                    }
                }
            } else if (scan_code == 0xB7) {
                if (get_code_from_kbuf() == 0xE0) {
                    if (get_code_from_kbuf() == 0xAA) {
                        key = PRINTSCREEN;
                        is_make = false;
                    }
                }
            }
            if (key == 0) {
                code_E0 = 1;
            }
        }
        if ((key != PAUSEBREAK) && (key != PRINTSCREEN)) {
            is_make = (scan_code & FLAG_BREAK) ? false : true;
            keyrow = &keymap[(scan_code & 0x7F) * MAP_COLS];
            column = 0;

            if (shift_l || shift_r) {
                column = 1;
            }
            if (code_E0) {
                column = 2;
                code_E0 = 0;
            }
            key = keyrow[column];

            switch (key) {
            case SHIFT_L:
                shift_l = is_make;
                break;
            case SHIFT_R:
                shift_r = is_make;
                break;
            case CTRL_L:
                ctrl_l = is_make;
                break;
            case CTRL_R:
                ctrl_r = is_make;
                break;
            case ALT_L:
                alt_l = is_make;
                break;
            case ALT_R:
                alt_r = is_make;
                break;
            default:
                break;
            }
            if (is_make) {
                key |= shift_l ? FLAG_SHIFT_L : 0;
                key |= shift_r ? FLAG_SHIFT_R : 0;
                key |= ctrl_l ? FLAG_CTRL_L : 0;
                key |= ctrl_r ? FLAG_CTRL_R : 0;
                key |= alt_l ? FLAG_ALT_L : 0;
                key |= alt_r ? FLAG_ALT_L : 0;

                in_process(key);
            }
        }
    }
}

static uint8
get_code_from_kbuf()
{
    uint8 code;

    while (kb_buf.size <= 0) { }

    disable_int();
    code = *(kb_buf.tail);
    ++kb_buf.tail;
    if (kb_buf.tail == kb_buf.buf + KB_MAX_SIZE) {
        kb_buf.tail = kb_buf.buf;
    }
    --kb_buf.size;
    enable_int();

    return code;
}
