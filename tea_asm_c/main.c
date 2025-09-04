#include "data.h"

#define BLOCK_SIZE 8

/*
Funciones externas de ensamblador

tea_encrypt: Cifra un bloque de 64 bits usando la clave proporcionada.
Params:
    - v (uint32_t[2]): El bloque de datos a cifrar (dos palabras de 32 bits).
    - key (const uint32_t[4]): La clave de cifrado (cuatro palabras de 32 bits).
Returns:
    - None 

tea_decrypt: Descifra un bloque de 64 bits usando la clave proporcionada.
Params:
    - v (uint32_t[2]): El bloque de datos a descifrar (dos palabras de 32 bits).
    - key (const uint32_t[4]): La clave de descifrado (cuatro palabras de 32 bits).
*/
extern void tea_encrypt(uint32_t v[2], const uint32_t key[4]);
extern void tea_decrypt(uint32_t v[2], const uint32_t key[4]);

/*
Function: print_char
Imprime un carácter en la salida estándar (simulado aquí con una dirección de memoria específica).

Params:
    - c (char): El carácter a imprimir.

Returns:
    - None
*/
void print_char(char c) {
    // In a real bare-metal environment, this would write to UART
    // For now, this is just a placeholder
    volatile char *uart = (volatile char*)0x10000000;
    *uart = c;
}

/*
Function: print_number
Imprime un número entero en la salida estándar.

Params:
    - num (int): El número a imprimir.

Returns:
    - None
*/
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

/*
Function: print_string
Imprime una cadena de caracteres en la salida estándar.

Params:
    - str (const char*): La cadena a imprimir.

Returns:
    - None
*/
void print_string(const char* str) {
    while (*str) {
        print_char(*str++);
    }
}

/*
Function: print_hex
Imprime un buffer de bytes en formato hexadecimal.

Params:
    - buffer (const char*): El buffer de bytes a imprimir.
    - len (size_t): La longitud del buffer.

Returns:
    - None
*/
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

/*
Function: print_ascii
Imprime un buffer de bytes en formato ASCII.

Params:
    - buffer (const char*): El buffer de bytes a imprimir.
    - len (size_t): La longitud del buffer.

Returns:
    - None
*/
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


/*
Function: PKCS7
Aplica el esquema de padding PKCS#7 a un bloque de datos.

Params:
    - input (const char*): El bloque de datos original.
    - inputLen (size_t): La longitud del bloque de datos original.
    - output (char*): El buffer donde se almacenará el bloque con padding.

Returns:
    - size_t: La longitud del bloque con padding.
*/
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

/*
Function: remove_PKCS7
Remueve el padding PKCS#7 de un bloque de datos.

Params:
    - data (char*): El bloque de datos con padding.
    - inputLen (size_t): La longitud del bloque de datos con padding.

Returns:
    - size_t: La longitud del bloque sin padding.
*/
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

/*
Function: bytes_to_words
Convierte 8 bytes en dos palabras de 32 bits (big-endian).

Params:
    - bytes (const char*): Los bytes a convertir.
    - v (uint32_t*): El buffer donde se almacenarán las palabras de 32 bits.

Returns:
    - None
*/
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

/*
Function: words_to_bytes
Convierte dos palabras de 32 bits (big-endian) en 8 bytes.

Params:
    - v (const uint32_t*): Las palabras de 32 bits a convertir.
    - bytes (char*): El buffer donde se almacenarán los bytes.

Returns:
    - None
*/
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

/*
Function: main
Programa principal que demuestra el cifrado y descifrado usando TEA con padding PKCS#7.
*/
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