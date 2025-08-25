#pragma once
#include <stdint.h>

void __assert_failed(const char* assert, const char* file, const char* function, uint32_t line);

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define assert(expression) ((expression) ? (void)0 : __assert_failed(#expression, __FILENAME__, __func__, __LINE__))