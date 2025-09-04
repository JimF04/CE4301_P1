.section .text
.globl _start
_start:
    # cargar v0 y v1 desde memoria
    la t0, v0
    lw s0, 0(t0)
    la t1, v1
    lw s1, 0(t1)

    # cargar delta
    la t2, delta
    lw s2, 0(t2)

    # cargar key
    la t3, key
    lw s3, 0(t3)
    lw s4, 4(t3)
    lw s5, 8(t3)
    lw s6, 12(t3)

    # contador de rondas
    li s8, 32
	
    # iniciar sum = DELTA * 32
    li s7, 0
    mul s7, s2, s8

tea_decrypt_loop:

    # v1 -= ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3])
    sll t0, s0, 4
    add t0, t0, s5
    add t1, s0, s7
    xor t0, t0, t1
    srl t1, s0, 5
    add t1, t1, s6
    xor t0, t0, t1
    sub s1, s1, t0

    # v0 -= ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1])
    sll t0, s1, 4
    add t0, t0, s3
    add t1, s1, s7
    xor t0, t0, t1
    srl t1, s1, 5
    add t1, t1, s4
    xor t0, t0, t1
    sub s0, s0, t0

    sub s7, s7, s2 # sum -= delta

    # disminuir contador
    addi s8, s8, -1
    bnez s8, tea_decrypt_loop

    # almacenar resultados
    la t0, v0
    sw s0, 0(t0)
    la t1, v1
    sw s1, 0(t1)

end:
    j end

key:
    .word 0x12345678
    .word 0x9ABCDEF0
    .word 0xFEDCBA98
    .word 0x76543210
v0:
    .word 0xBE91739F # HOLA
v1:
    .word 0xC5B30D30 # 1234
delta:
    .word 0x9E3779B9 # delta