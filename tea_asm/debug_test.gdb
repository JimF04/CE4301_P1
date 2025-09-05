file encrypt.elf
target remote :1234
break _start
break end
layout asm
layout regs
continue
continue
monitor quit
quit
y