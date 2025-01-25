#include <libc/string.h>

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

void itoa(int32_t num, char* str) {
    uint32_t index = 0;

    do {
        str[index++] = (num % 10) + '0';
        num /= 10;
    } while(num != 0);

    str[index] = '\0';

    reverse(str);
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