#pragma once

#include <cstddef>
#include <cstdint>

namespace Neon {
extern "C" {
	size_t strlen(const char *str);
	char *strcpy(char *destination, const char *source);
	char *strcat(char *destination, const char *source);
	int strcmp(const char *a, const char *b);
	char *strstr(const char *str, const char *substring);
	char *strchr(const char str[], char ch);
	void memcpy(void *dest, const void *src, size_t n);
	void memset(void *str, char ch, size_t n);
	void memmove(void *dest, const void *src, size_t n);
	int memcmp(const void *s1, const void *s2, size_t n);
	char *strpbrk(const char *str1, const char *str2);
}
} // name