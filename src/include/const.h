#ifndef _CONST_H_
#define _CONST_H_

#define NAME_LEN 20
#define TASK_NUM 1
#define PROCESS_NUM 2
#define IRQ_NUM 16
#define SYS_CALL_NUM 2
#define CONSOLE_NUM 3

#define CLOCK_IRQ       0
#define KEYBOARD_IRQ    1
#define CASCADE_IRQ     2       /* cascade enable for 2nd AT controller */
#define ETHER_IRQ       3       /* default ethernet interrupt vector */
#define SECONDARY_IRQ   3       /* RS232 interrupt vector for port 2 */
#define RS232_IRQ       4       /* RS232 interrupt vector for port 1 */
#define XT_WINI_IRQ     5       /* xt winchester */
#define FLOPPY_IRQ      6       /* floppy disk */
#define PRINTER_IRQ     7
#define AT_WINI_IRQ     14      /* at winchester */

#define DEFAULT_CHAR_COLOR 0x07

#define SCREEN_WIDTH 80
#define SCREEN_SIZE (80 * 25)

#endif /* _CONST_H_ */
