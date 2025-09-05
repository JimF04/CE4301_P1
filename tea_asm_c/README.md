# Desarrollo: C + Assembly

Este desarrollo demuestra cómo un programa en C puede integrarse con funciones criptográficas escritas en ensamblador RISC-V, en este caso la implementación del algoritmo TEA (Tiny Encryption Algorithm).

## Archivos

- `main.c`: Programa principal en C. Gestiona cadenas, aplica padding PKCS#7, convierte datos entre bytes y palabras de 32 bits, y realiza llamadas a las funciones en ensamblador.
- `data.h` : Header donde están definidos la clave y la cadena a encriptar.
- `startup.s`: Código de inicio que configura la pila y transfiere el control a `main()`.
- `tea_encrypt.s`: Implementación en ensamblador de la función de **cifrado TEA**.
- `tea_decrypt.s`: Implementación en ensamblador de la función de **descifrado TEA**.
- `linker.ld`: Script de enlazado que define la memoria y el punto de entrada.
- `build.sh`: Script de compilación.
- `run-qemu.sh`: Script para ejecutar QEMU con soporte de depuración.
- `simple_debug.gdb`: Script de comandos para depuración rápida en GDB.

## Funcionalidad

### Programa en C (`main.c`)
- Implementa funciones auxiliares para imprimir caracteres, números, cadenas, hexadecimales y ASCII.
- Aplica **padding PKCS#7** a los datos de entrada.
- Llama a `tea_encrypt` y `tea_decrypt` implementados en ensamblador.
- Imprime los bloques cifrados/descifrados en formato ASCII y HEX.
- Elimina el padding después de descifrar para recuperar el mensaje original.

### Código de inicio (`startup.s`)
- Establece el puntero de pila (sp) al inicio del área de pila definida en linker.ld
- Llama a la función main() del programa C
- Proporciona un bucle infinito si main() retorna, evitando que el programa termine inesperadamente

### Función en Assembly (`tea_encrypt.s`)
- Implementa el **cifrado TEA** sobre un bloque de 64 bits (dos enteros de 32 bits).
- Recibe parámetros mediante los registros de llamada (`a0`: puntero a bloque, `a1`: puntero a clave).
- Respeta las convenciones de llamada RISC-V (salvado y restauración de registros).

### Función en Assembly (`tea_decrypt.s`)
- Implementa el **descifrado TEA**, proceso inverso al cifrado.
- Recibe los mismos parámetros que `tea_encrypt`.
- También respeta las convenciones de llamada RISC-V.

## Compilación y ejecución

```bash
# Compilar
cd /home/rvqemu-dev/workspace/tea_asm_c
./build.sh

# Ejecutar con QEMU (en una terminal)
./run-qemu.sh
```

## Depuración con GDB

```bash
# En otra terminal: ejecutar script de GDB
docker exec -it rvqemu /bin/bash
cd /home/rvqemu-dev/workspace/tea_asm_c
gdb-multiarch main.elf 
```

Para facilitar la depuración, puede usar los comandos archivo `degub_test.gdb`:

```gdb
file encrypt.elf 
target remote :1234
break end_tea_encrypt_loop
break end_tea_decrypt_loop
layout asm
layout regs
continue
continue
monitor quit
quit
```

## Convenciones de llamada RISC-V

* `a0`: Primer parámetro de entrada / valor de retorno
* `a1-a7`: Parámetros adicionales
* `s0-s11`: Registros salvados (preservados)
* `t0-t6`: Registros temporales
* `ra`: Dirección de retorno
* `sp`: Puntero de pila

Las funciones `tea_encrypt` y `tea_decrypt` cumplen estas reglas:

1. Guardan en la pila los registros que modifican.
2. Reciben parámetros en `a0` y `a1`.
3. Devuelven resultados en `a0` (modificando directamente el bloque en memoria).
4. Restauran registros antes de retornar.

---
### Detalles de Desarrollo

- [`tea_asm/README.md`](../tea_asm/README.md) - Ensamblador puro
- [`README.md`](../README.md) - Documentación General

---