#include <Cxx/cstdint>
#include <Cxx/cstddef>

namespace Neon {
    size_t strlen(const char* str) {
        size_t length = 0;
        while(str[length]) {
            length++;
        }
        return length;
    }

    char* strcpy(char* destination, const char* source) {
        if (destination == NULL) {
            return NULL;
        }
        char* ptr = destination;
        while(*source != '\0') {
            *destination = *source;
            destination++;
            source++;
        }
        *destination = '\0';
        return ptr;
    }

    char* strcat(char* destination, const char* source) {
        char* ptr = destination + strlen(destination);
        while(*source != '\0') {
            *ptr++ = *source++;
        }
        *ptr = '\0';
        return destination;
    }

    int strcmp(const char* a, const char* b) {
        while (*a && *a == *b) { a++; b++; }
        return (int)(unsigned char)(*a) - (int)(unsigned char)(*b);
    }

    char* strstr(const char *str, const char *substring) {
        const char *a = str, *b = substring;
        for (;;) {
            if ( !*b ) return (char *) str;
            if ( !*a ) return NULL;
            if ( *a++ != *b++) { a = ++str; b = substring; }
        }
    }

    char* strchr(const char str[], char ch) {
        while (*str && *str != ch ) ++str;
        return (char *)(ch == *str ? str : NULL);  
    }

    void memcpy(void* dest, void* src, size_t n) {
        int i;
        char* src_char = (char *)src;
        char* dest_char = (char *)dest;
        for (i = 0; i < n; i++) {
            dest_char[i] = src_char[i];
        }
    }

    void memset(void* str, char ch, size_t n) {
  	    int i;
	    char* s = (char *) str;
	    for(i = 0; i < n; i++) {
            s[i] = ch;
        }
    }

    void memmove(void* dest, void* src, size_t n) {
        char *csrc = (char *)src;
        char *cdest = (char *)dest;
        char temp[n];
        for (int i = 0; i < n; i++) {
            temp[i] = csrc[i];
        }
        for (int i = 0; i < n; i++) {
            cdest[i] = temp[i];
        }
    }
}