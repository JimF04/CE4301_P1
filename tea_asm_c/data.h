#ifndef DATA_H
#define DATA_H
#include <stddef.h>
#include <stdint.h>

// TEA key (128 bits)
static const uint32_t key[4] = {
    0x12345678, 0x9ABCDEF0, 0xFEDCBA98, 0x76543210
};

// Input message
static const char input[] = "HOLA1234 como estas me llamo jimmy";

#endif