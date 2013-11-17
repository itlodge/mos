#ifndef _TYPE_H_
#define _TYPE_H_

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef unsigned char bool;

#define true 1
#define false 0

typedef void (*int_handler)();
typedef void (*task_func)();
typedef void (*irq_handler)(int irq);

typedef void * system_call;
typedef char * va_list;

#endif /* _TYPE_H_ */
