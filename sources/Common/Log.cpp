#include <Common/Log.h>

namespace Neon {
    void (*limine_print)(const char *buf, size_t size) = NULL;

    static const char CONVERSION_TABLE[] = "0123456789abcdef";

    void e9_putc(char c) {
        if (limine_print != NULL) limine_print(&c, 1);
        __asm__ __volatile__ ("outb %0, %1" :: "a" (c), "Nd" (0xe9) : "memory");
    }

    void e9_print(const char *Message) {
        for (size_t i = 0; Message[i]; i++) {
            e9_putc(Message[i]);
        }
    }

    void e9_puts(const char *msg) {
        e9_print(msg);
        e9_putc('\n');
    }

    static void e9_printhex(size_t num) {
        int i;
        char buf[17];

        if (!num) {
            e9_print("0x0");
            return;
        }

        buf[16] = 0;

        for (i = 15; num; i--) {
            buf[i] = CONVERSION_TABLE[num % 16];
            num /= 16;
        }

        i++;
        e9_print("0x");
        e9_print(&buf[i]);
    }

    static void e9_printdec(size_t num) {
        int i;
        char buf[21] = {0};

        if(!num) {
            e9_putc('0');
            return;
        }

        for(i = 19; ; i--) {
            buf[i] = (num % 10) + 0x30;
            num = num / 10;
        }

        i++;
        e9_print(buf + i);
    }

    void Log(const char *Format, ...) {
        va_list argp;
        va_start(argp, Format);

        while (*Format != '\0') {
            if (*Format == '%') {
                Format++;
                if (*Format == 'x') {
                    e9_printhex(va_arg(argp, size_t));
                } else if (*Format == 'd') {
                    e9_printdec(va_arg(argp, size_t));
                } else if (*Format == 's') {
                    e9_print(va_arg(argp, char*));
                }
            } else {
                e9_putc(*Format);
            }
            Format++;
        }

        e9_putc('\n');
        va_end(argp);
    }

    void Warn(const char *Message, ...) {
        Log("\x1b[33m[WARN]\x1b[33m %s", Message);
        Log("\x1b[0m");
    }

    void Error(const char *Message, ...) {
        Log("\x1b[31m[ERROR]\x1b[31m %s", Message);
        Log("\x1b[0m");
    }
}