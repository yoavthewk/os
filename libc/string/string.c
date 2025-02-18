#include <libc/string.h>
#include <stdbool.h>

uint32_t strlen(char* str){
    uint32_t len = 0;
    while(str[++len]);
    return len;
}

void reverse(char* str) {
    uint32_t beginning = 0;
    uint32_t end = strlen(str) - 1;
    char temp;

    while(end > beginning) {
        temp = str[beginning];
        str[beginning] = str[end];
        str[end] = temp;

        --end;
        ++beginning;
    }
}

void itoa(int32_t num, char* buffer, int base) {
    uint32_t index = 0;
    bool is_negative = 0;

    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }

    do {
        buffer[index++] = (num % base) < 10 ? (num % base) + '0' : ((num % base) - 10) + 'A';
        num /= base;
    } while(num != 0);

    if (is_negative) buffer[index++] = '-';

    buffer[index] = '\0';

    reverse(buffer);
}

uint8_t strcmp(char* first, char* second) {
    if (strlen(first) != strlen(second)) {
        if (strlen(first) > strlen(second)) {
            return 1;
        }
        else {
            return 2;
        }
    }

    for (uint32_t index = 0; first[index] != 0; ++index) {
        if (first[index] != second[index]) {
            return 3;
        }
    }

    return 0;
}

void strcpy(char* src, char* dst) {
    for (uint32_t index = 0; src[index] != 0; ++index) {
        dst[index] = src[index];
    }
}
