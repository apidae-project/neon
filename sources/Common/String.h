#pragma once

#include <cstdint>
#include <cstddef>

namespace Neon {
    extern "C" {
        size_t strlen(const char *str);
        char *strcpy(char *destination, const char *source);
        char *strcat(char *destination, const char *source);
        int strcmp(const char *a, const char *b);
        char *strstr(const char *str, const char *substring);
        char *strchr(const char str[], char ch);
        void memcpy(void *dest, void *src, size_t n);
        void memset(void *str, char ch, size_t n);
        void memmove(void *dest, void *src, size_t n);
    }
}