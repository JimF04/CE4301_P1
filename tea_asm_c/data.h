#ifndef DATA_H
#define DATA_H
#include <stddef.h>
#include <stdint.h>

// Input message
static const char input[] = "Mensaje de prueba para TEA"; 

// TEA key (128 bits)
static const uint32_t key[4] = {
    //0x12345678, 0x9ABCDEF0, 0xFEDCBA98, 0x76543210
    0xA56BABCD, 0x000FF123, 0xDEADBEEF, 0x01234567
};

#endif