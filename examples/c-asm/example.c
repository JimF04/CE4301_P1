#include <stddef.h> 
#include <stdint.h>  

#define BLOCK_SIZE 8

// Assembly function declaration
extern int sum_to_n(int n);

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
        print_char(hex_chars[(buffer[i] >> 4) & 0xF]);
        print_char(hex_chars[buffer[i] & 0xF]);
        print_char(' ');
    }
    print_char('\n');
}

void print_ascii(const char* buffer, size_t len) {
    for (size_t i = 0; i < len; i++) {
        char c = buffer[i];
        if (c >= 32 && c <= 126) { // caracteres imprimibles
            print_char(c);
        } else {
            print_char('.'); // reemplazar no imprimibles por '.'
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

// Entry point for C program
void main() {
    const char input[] = "HOLA1234";
    size_t inputLen = sizeof(input) - 1;   // 8 bytes
    char padded[inputLen + BLOCK_SIZE];
    size_t paddedLen = PKCS7(input, inputLen, padded);

    print_string("Original message:\n");
    print_ascii(input, inputLen);

    print_string("Padded data (hex):\n");
    print_hex(padded, paddedLen);

    print_string("Padded data (ASCII):\n");
    print_ascii(padded, paddedLen);

    while (1) { __asm__ volatile ("nop"); }
}