<!-- My notes, for later checking -->

Assembly
==========
Registers
-----------
*cs*: Code segment register

*ds*: Data segment register

*es*: Extended segment register

*fs*:

*gs*: No specific, often used as "Graphic register"

*ss*: Stack segment register

*bp*: Base pointer register

*si*: source index register

*di*: destination index register

Operations
------------
*cli*: Close interrupt

*sti*: Start interrupt

*movzx*: Pass the value of the 16-bit register to the 32-bit register

*cld*: Clear direction flag, DF=0(Address increase from low to high)

*std*: Set direction flag, DF=1(Address increase from high to low)

*lodsb*: Load byte from DS:(E)SI to AL

*lodsw*: Load word from DS:(E)SI to AX

*lodsd*: Load double word from DS:(E)SI to EAX

*stosb*: Store byte in AL to ES:(E)DI

*stosw*: Store word in AX to ES:(E)DI

*stosd*: Store double word in EAX to ES:(E)DI

*div*: AX, 8-bit division, reminder is in AH, quotient is in AL
DX:AX, 16-bit division, reminder is in DX, quotient is in AX

*resb*: Reserve bytes

*resw*: Reserve words

*popfw*: Pops a word form the stack and store it in the bottom 16 bits of the
flags register

*popfd*: Pops a double word form the stack and store it in the bottom 16 bits
of the flags register

*hlt*: Halt

*lea*: Load effective address, calculates the effective address of the 2nd
operand and stores the result in the 1st operand.

*iretd*: Return from an interrupt, pop IP, CS ... and continue CS:IP

*pusha*: store the entire integer register

*popa*: retrieve the entire integer register

*pushf*: store the (E)FLAGS register

*popf*: retrieve the (E)FLAGS register

Interrupts
-------------
*10h*: For screen

*13h*: For disk

Flags
-----------
*CF*: Carray flag

*DF*: Direction flag

*OF*: Overflow flag


Others
-----------
xor eax eax    ; Set to be zero

$: Current position

$$: The start address of the current section

In 32-bit machine,

byte: 8 bits

word: 16 bits

dword: 32 bits

The first bit of register **cr0** is PE. If PE is 0, CPU will running in real
mode. If PE is 1, CPU will running in protect mode.

The screen is from (00, 00) to (24, 79)

Problems
===========
* p26, line 40 (shl eax, 4), 4 bits or 4 bytes? line 43 (shr eax, 16),
   16 bits or 16 bytes?


Typo
==========
* p121, code list 5.2, line 6, should be foo.o instead of hello.o

* p140, code list 5.22, line 718, comment "OffsetOfKernel" should be
"BaseOfKernel"
   
* p284, code list 7.44, line 339, ``kb_read =! KB_ACK`` should be
``kb_read != KB_ACK``.
