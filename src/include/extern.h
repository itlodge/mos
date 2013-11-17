/***************************************************************
 * This header file declare the common used function defined in
 * lib/klib.asm or lib/klibc.c
 ***************************************************************/

extern void
enable_irq(int irq);

extern void
disable_irq(int irq);

extern uint8
in_byte(uint16 port);

extern void
out_byte(uint16 port, uint8 value);

extern void
disable_int();

extern void
enable_int();

extern int
get_ticks();

extern void
write(char *buf, int len);

// Display 
char *
uitoa(char *str, int n);

extern void
disp_int(int n);

extern void
disp_str(const char *str);

extern void
disp_color_str(const char *str, int color);

extern void *
memcpy(void *dst, void *src, int size);

extern void
memset(void *dst, char ch, int size);

extern char *
strcpy(char *dst, const char *src);

extern int
strlen(char *str);

extern void
delay(int t);

extern int
printf(const char *format, ...);

// Variable
extern int disp_pos;

