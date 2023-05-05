#include <cstddef>
#include <cstdint>

namespace Neon {
extern "C" {
	size_t strlen(const char *String) {
		size_t Length = 0;
		while (*String++) Length++;
		return Length;
	}

	char *strcpy(char *destination, const char *source) {
		if(destination == nullptr) return nullptr;
		char *ptr = destination;
		while(*source != '\0') {
			*destination = *source;
			destination++;
			source++;
		}
		*destination = '\0';
		return ptr;
	}

	char *strcat(char *destination, const char *source) {
		char *ptr = destination + strlen(destination);
		while (*source != '\0') { *ptr++ = *source++; }
		*ptr = '\0';
		return destination;
	}

	int strcmp(const char *a, const char *b) {
		while(*a && *a == *b) {
			++a;
			++b;
		}
		return (int)(unsigned char)(*a) - (int)(unsigned char)(*b);
	}

	char *strstr(const char *str, const char *substring) {
		const char *a = str, *b = substring;
		for(;;) {
			if(!*b) return (char *)str;
			if(!*a) return nullptr;
			if(*a++ != *b++) {
				a = ++str;
				b = substring;
			}
		}
	}

	char *strchr(const char str[], char ch) {
		while(*str && *str != ch) ++str;
		return (char *)(ch == *str ? str : nullptr);
	}

	void memcpy(void *dest, const void *src, size_t n) {
		int i;
		char *src_char = (char *)src;
		char *dest_char = (char *)dest;
		for(i = 0; i < n; i++) { dest_char[i] = src_char[i]; }
	}

	void memset(void *str, char ch, size_t n) {
		int i;
		char *s = (char *)str;
		for(i = 0; i < n; i++) { s[i] = ch; }
	}

	void memmove(void *dest, const void *src, size_t n) {
		char *csrc = (char *)src;
		char *cdest = (char *)dest;
		char temp[n];
		for(int i = 0; i < n; i++) { temp[i] = csrc[i]; }
		for(int i = 0; i < n; i++) { cdest[i] = temp[i]; }
	}

	int memcmp(const void *s1, const void *s2, size_t n) {
		const auto *p1 = reinterpret_cast<const unsigned char *>(s1),
		           *p2 = reinterpret_cast<const unsigned char *>(s2);
		while(n-- > 0) {
			if (*p1 != *p2) return (*p1 > *p2) ? 1 : -1;
			p1++;
			p2++;
		}
		return 0;
	}

	char *strpbrk(const char *str1, const char *str2) {
		for(const char *p = str1; *p; ++p) if(strchr(str2, *p)) return const_cast<char *>(p);
		return nullptr;
	}
}
} // namespace Neon
