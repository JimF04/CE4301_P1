# Conectar a QEMU (RISC-V)
target remote :1234

# Colocar breakpoints
break _start              
break main                
break PKCS7                          

# Layouts para debugging visual
layout asm                
layout regs              

# Iniciar ejecución
continue                  

# Paso a paso dentro de main
continue

# Llamada a PKCS7
continue                 

# Continuar hasta el final
continue

# Salir 
monitor quit
quit