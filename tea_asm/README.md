# ASM TEA Encrypt / Decrypt

Programa simple del algoritmo **TEA (Tiny Encryption Algorithm)** escrito completamente en ensamblador **RISC-V**. Incluye dos versiones:

* `encrypt.s` 
* `decrypt.s` 

---

## Archivos

* `encrypt.s`: Código fuente en ensamblador para **encriptar**.
* `decrypt.s`: Código fuente en ensamblador para **desencriptar**.
* `linker.ld`: Script de enlazado.
* `build.sh`: Script de compilación.
* `run-qemu.sh`: Script para ejecutar QEMU.

---

## Funcionalidad

### TEA Encrypt (`test.s`)

* Encripta 2 bloques de 32 bits (`v0` y `v1`).
* Inicializa registros con valores.
* Realiza 32 rondas de suma y mezcla usando la **clave de 128 bits**.
* Termina en un bucle infinito para mantener resultados en memoria.

### TEA Decrypt (`test2.s`)

* Desencripta 2 bloques de 32 bits previamente cifrados.
* Inicializa registros con valores encriptados.
* Realiza 32 rondas inversas para recuperar el mensaje original.

> ⚠️ **Importante:**
> Debes escoger cuál ensamblador compilar y ejecutar:
>
> * `encrypt.s` → encriptación
> * `decrypt.s` → desencriptación

---

## Configuración del mensaje y clave

En el **código fuente ensamblador**, debes **agregar manualmente la clave y los bloques de datos (`v0` y `v1`) en hexadecimal**. Por ejemplo:

```asm
key:
    .word 0x12345678
    .word 0x9ABCDEF0
    .word 0xFEDCBA98
    .word 0x76543210

v0:
    .word 0x484F4C41 # "HOLA"
v1:
    .word 0x31323334 # "1234"

delta:
    .word 0x9E3779B9 # constante delta TEA
```

> Esto permite definir el mensaje a cifrar o descifrar directamente en memoria antes de iniciar el algoritmo.

---

## Compilación y ejecución

```bash
# Compilar
./build.sh  # ajusta si quieres compilar encrypt.s o decrypt.s

# Ejecutar con QEMU (en una terminal)
./run-qemu.sh

# En otra terminal, conectar GDB
docker exec -it rvqemu /bin/bash
cd /home/rvqemu-dev/workspace/tea_asm
gdb-multiarch encrypt.elf   # o decrypt.elf según el archivo
```

---

## Depuración con GDB

```gdb
target remote :1234
break _start
continue
layout asm
layout regs
step  # o 's' para ejecutar paso a paso
monitor quit # para terminar la aplicación de QEMU
quit # para salir
```

---

## Registros importantes

* `t0-t1`: Variables temporales para cálculos intermedios.
* `s0` (`v0`) / `s1` (`v1`): Bloques de datos que se encriptan/desencriptan.
* `s2` (`delta`): Constante de mezcla TEA.
* `s3-s6` (`key[0-3]`): Clave de 128 bits.
* `s7`: Suma acumulativa (`sum`)
* `s8`: Contador de rondas

---

## Ejemplo de elección

* Para **encriptar** el mensaje `"HOLA1234"`: usa `encrypt.s`.
* Para **desencriptar** el mensaje cifrado: usa `decrypt.s`.

---
