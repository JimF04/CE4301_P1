#include "data.h"

#define BLOCK_SIZE 8

// Assembly function declaration
extern int sum_to_n(int n);
extern void tea_encrypt(uint32_t v[2], const uint32_t key[4]);
extern void tea_decrypt(uint32_t v[2], const uint32_t key[4]);

// Simple implementation of basic functions since we're in bare-metal environment
void print_char(char c) {
    // In a real bare-metal environment, this would write to UART
    // For now, this is just a placeholder
    volatile char *uart = (volatile char*)0x10000000;
    *uart = c;
}

void print_number(int num) {
    if (num == 0) {
        print_char('0');
        return;
    }
    
    if (num < 0) {
        print_char('-');
        num = -num;
    }
    
    char buffer[10];
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // Print digits in reverse order
    while (i > 0) {
        print_char(buffer[--i]);
    }
}

void print_string(const char* str) {
    while (*str) {
        print_char(*str++);
    }
}

void print_hex(const char* buffer, size_t len) {
    const char hex_chars[] = "0123456789ABCDEF";
    for (size_t i = 0; i < len; i++) {
        unsigned char b = (unsigned char)buffer[i];
        print_char(hex_chars[(b >> 4) & 0xF]);
        print_char(hex_chars[b & 0xF]);
        print_char(' ');
    }
    print_char('\n');
}


void print_ascii(const char* buffer, size_t len) {
    for (size_t i = 0; i < len; i++) {
        char c = buffer[i];
        if (c >= 32 && c <= 126) {
            print_char(c);
        } else {
            print_char('.');
        }
    }
    print_char('\n');
}


// Padding PKC#7
size_t PKCS7(const char* input, size_t inputLen, char* output) {
    // Calcular tamaño de padding
    size_t rest = inputLen % BLOCK_SIZE; // Resto de la división
    size_t paddingSize = (rest == 0) ? BLOCK_SIZE : BLOCK_SIZE - rest; 

    // copiar datos originales
    for (size_t i = 0; i < inputLen; i++){
        output[i] = input[i]; 
    }

    // Agregar padding
    for (size_t i = 0; i < paddingSize; i++) {
        output[inputLen + i] = (char)paddingSize;
    }

    return inputLen + paddingSize;
} 

// Remover padding PKC#7
size_t remove_PKCS7(char* data, size_t inputLen) {
    if (inputLen == 0) return 0;

    unsigned char paddingSize = (unsigned char)data[inputLen - 1];
    if (paddingSize > BLOCK_SIZE || paddingSize == 0) {
        // Invalid padding
        return inputLen;
    }

    // Verificar que todos los bytes de padding sean iguales a paddingSize
    for (size_t i = 0; i < paddingSize; i++) {
        if ((unsigned char)data[inputLen - 1 - i] != paddingSize) {
            // Invalid padding
            return inputLen;
        }
    }

    return inputLen - paddingSize;
}

// Convertir 8 bytes a dos palabras de 32 bits (big-endian)
void bytes_to_words(const char* bytes, uint32_t* v) {
    v[0] = ((uint32_t)(unsigned char)bytes[0] << 24) |
           ((uint32_t)(unsigned char)bytes[1] << 16) |
           ((uint32_t)(unsigned char)bytes[2] << 8)  |
           ((uint32_t)(unsigned char)bytes[3]);
    v[1] = ((uint32_t)(unsigned char)bytes[4] << 24) |
           ((uint32_t)(unsigned char)bytes[5] << 16) |
           ((uint32_t)(unsigned char)bytes[6] << 8)  |
           ((uint32_t)(unsigned char)bytes[7]);
}

// Volver a escribir dos palabras de 32 bits en 8 bytes (big-endian)
void words_to_bytes(const uint32_t* v, char* bytes) {
    bytes[0] = (v[0] >> 24) & 0xFF;
    bytes[1] = (v[0] >> 16) & 0xFF;
    bytes[2] = (v[0] >> 8) & 0xFF;
    bytes[3] = v[0] & 0xFF;

    bytes[4] = (v[1] >> 24) & 0xFF;
    bytes[5] = (v[1] >> 16) & 0xFF;
    bytes[6] = (v[1] >> 8) & 0xFF;
    bytes[7] = v[1] & 0xFF;
}

// Entry point for C program
void main() {
    size_t inputLen = sizeof(input) - 1;   
    char padded[inputLen + BLOCK_SIZE];
    size_t paddedLen = PKCS7(input, inputLen, padded);

    print_char('\n');
    print_string("Original message:\n");
    print_ascii(input, inputLen);

    print_char('\n');
    print_string("Encrypting...\n");

    uint32_t v[2];
    for (size_t i = 0; i < paddedLen; i += BLOCK_SIZE) {
        bytes_to_words(&padded[i], v);
        tea_encrypt(v, key);
        words_to_bytes(v, &padded[i]);
    }

    // Print a cada bloque HEX
    print_string("Encrypted block (hex):\n");
    for (size_t i = 0; i < paddedLen; i += BLOCK_SIZE) {
        print_hex(&padded[i], BLOCK_SIZE);
    }

    // Print a cada bloque ASCII
    print_string("Encrypted block (ASCII):\n");
    for (size_t i = 0; i < paddedLen; i += BLOCK_SIZE) {
        print_ascii(&padded[i], BLOCK_SIZE);
    }

    print_string("Encrypted message:\n");
    print_ascii(padded, paddedLen);

    print_char('\n');
    print_string("Decrypting...\n");

    for (size_t i = 0; i < paddedLen; i += BLOCK_SIZE) {
        bytes_to_words(&padded[i], v);
        tea_decrypt(v, key);
        words_to_bytes(v, &padded[i]);
    }

    // Print a cada bloque HEX
    print_string("Decrypted block (hex):\n");
    for (size_t i = 0; i < paddedLen; i += BLOCK_SIZE) {
        print_hex(&padded[i], BLOCK_SIZE);

    }

    // Print a cada bloque ASCII
    print_string("Decrypted block (ASCII):\n");
    for (size_t i = 0; i < paddedLen; i += BLOCK_SIZE) {
        print_ascii(&padded[i], BLOCK_SIZE);
    }

    paddedLen = remove_PKCS7(padded, paddedLen);

    print_char('\n');
    print_string("Decrypted message:\n");
    print_ascii(padded, paddedLen);

    while (1) { __asm__ volatile ("nop"); }
}