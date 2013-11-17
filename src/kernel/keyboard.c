#include "keyboard.h"
#include "keymap.h"
#include "extern.h"
#include "tty.h"

// External functions from C files
extern void
put_irq_handler(int irq, irq_handler handler);

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

static void
kb_wait();

static void
kb_ack();

static void
set_leds();

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

    caps_lock = 0;
    num_lock = 1;
    scroll_lock = 0;

    set_leds();
    
    put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
    enable_irq(KEYBOARD_IRQ);
}

void
keyboard_read(Tty *tty)
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

            int caps = shift_l || shift_r;
            if (caps_lock) {
                if ((keyrow[0] >= 'a') && (keyrow[0] <= 'z')) {
                    caps = !caps;
                }
            }
            if (caps) {
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
            case CAPS_LOCK:
                if (is_make) {
                    caps_lock = !caps_lock;
                    set_leds();
                }
                break;
            case NUM_LOCK:
                if (is_make) {
                    num_lock = !num_lock;
                    set_leds();
                }
                break;
            case SCROLL_LOCK:
                if (is_make) {
                    scroll_lock = !scroll_lock;
                    set_leds();
                }
                break;
            default:
                break;
            }
            if (is_make) {
                int pad = 0;

                // Small keyboard
                if ((key >= PAD_SLASH) && (key <= PAD_9)) {
                    pad = 1;
                    switch (key) {
                    case PAD_SLASH:
                        key = '/';
                        break;
                    case PAD_STAR:
                        key = '*';
                        break;
                    case PAD_MINUS:
                        key = '-';
                        break;
                    case PAD_PLUS:
                        key = '+';
                        break;
                    case PAD_ENTER:
                        key = ENTER;
                        break;
                    default:
                        break;
                    }
                    if (num_lock && (key >= PAD_0) && (key <= PAD_9)) {
                        key -= PAD_0 + '0';
                    } else if (num_lock && key == PAD_DOT) {
                        key = '.';
                    } else {
                        switch (key) {
                        case PAD_HOME:
                            key = HOME;
                            break;
                        case PAD_END:
                            key = END;
                            break;
                        case PAD_PAGEUP:
                            key = PAGEUP;
                            break;
                        case PAD_PAGEDOWN:
                            key = PAGEDOWN;
                            break;
                        case PAD_INS:
                            key = INSERT;
                            break;
                        case PAD_UP:
                            key = UP;
                            break;
                        case PAD_DOWN:
                            key = DOWN;
                            break;
                        case PAD_LEFT:
                            key = LEFT;
                            break;
                        case PAD_RIGHT:
                            key = RIGHT;
                            break;
                        case PAD_DOT:
                            key = DELETE;
                            break;
                        default:
                            break;
                        }
                    }
                }
                            
                key |= shift_l ? FLAG_SHIFT_L : 0;
                key |= shift_r ? FLAG_SHIFT_R : 0;
                key |= ctrl_l ? FLAG_CTRL_L : 0;
                key |= ctrl_r ? FLAG_CTRL_R : 0;
                key |= alt_l ? FLAG_ALT_L : 0;
                key |= alt_r ? FLAG_ALT_L : 0;

                in_process(tty, key);
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

static void
kb_wait()
{
    uint8 status = in_byte(KB_CMD);

    while (status & 0x02) {
        status = in_byte(KB_CMD);
    }
}

static void
kb_ack()
{
    uint8 read = in_byte(KB_DATA);

    while (read != KB_ACK) {
        read = in_byte(KB_DATA);
    }
}

static void
set_leds()
{
    uint8 leds = (caps_lock << 2) | (num_lock << 1) | scroll_lock;

    kb_wait();
    out_byte(KB_DATA, LED_CODE);
    kb_ack();

    kb_wait();
    out_byte(KB_DATA, leds);
    kb_ack();
}

