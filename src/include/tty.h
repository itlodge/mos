#ifndef _TTY_H_
#define _TTY_H_

#include "console.h"

#define TTY_BUF_SIZE 256

typedef struct Tty {
    uint32 buf[TTY_BUF_SIZE];
    uint32 *buf_head;
    uint32 *buf_tail;
    int size;
    struct Console *console;
} Tty;

// Public functions
void
task_tty();

void
in_process(Tty *tty, uint32 key);


#endif /* _TTY_H_ */
