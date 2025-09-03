.section .text
.globl tea_encrypt
tea_encrypt:
    # Guardar registros
    addi sp, sp, -32
    sw ra, 28(sp)
    sw s0, 24(sp)
    sw s1, 20(sp)
    sw s2, 16(sp)
    sw s3, 12(sp)
    sw s4, 8(sp)
    sw s5, 4(sp)
    sw s6, 0(sp)

    # a0 = v
    lw s0, 0(a0) # v0 = v[0]
    lw s1, 4(a0) # v1 = v[1]

    # a1 = key
    lw s3, 0(a1)  # key[0] 
    lw s4, 4(a1)  # key[1]
    lw s5, 8(a1)  # key[2]
    lw s6, 12(a1) # key[3]
    
    li s2, 0 # sum = 0
    li t2, 0x9E3779B9 # delta
    li t3, 32 # contador

tea_encrypt_loop:
    add s2, s2, t2 # sum += delta

    # v0 += ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1])
    sll t0, s1, 4
    add t0, t0, s3
    add t1, s1, s2
    xor t0, t0, t1
    srl t1, s1, 5
    add t1, t1, s4
    xor t0, t0, t1
    add s0, s0, t0

    # v1 += ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3])
    sll t0, s0, 4
    add t0, t0, s5
    add t1, s0, s2
    xor t0, t0, t1
    srl t1, s0, 5
    add t1, t1, s6
    xor t0, t0, t1
    add s1, s1, t0

    # disminuir contador
    addi t3, t3, -1
    # verificar si el contador es cero
    bnez t3, tea_encrypt_loop

    # almacenar resultados
    sw s0, 0(a0)
    sw s1, 4(a0)

end_loop:

    # Restaurar registros y retornar
    lw s6, 0(sp)
    lw s5, 4(sp)
    lw s4, 8(sp)
    lw s3, 12(sp)
    lw s2, 16(sp)
    lw s1, 20(sp)
    lw s0, 24(sp)
    lw ra, 28(sp)
    addi sp, sp, 32
    ret