#include <Common/log.hxx>

namespace Print {
    void (*limine_print)(const char *buf, size_t size) = NULL;
    static const char CONVERSION_TABLE[] = "0123456789abcdef";

    void PutCharacter(char c) {
        if (limine_print != NULL) {
            limine_print(&c, 1);
        }
        __asm__ __volatile__ ("outb %0, %1" :: "a" (c), "Nd" (0xe9) : "memory");
    }

    void PutString(const char *msg) {
        for (size_t i = 0; msg[i]; i++) {
            PutCharacter(msg[i]);
        }
    }

    static void PutStringhex(size_t num) {
        int i;
        char buf[17];

        if (!num) {
        PutString("0x0");
        return;
    }

    buf[16] = 0;

    for (i = 15; num; i--) {
        buf[i] = CONVERSION_TABLE[num % 16];
        num /= 16;
    }

    i++;
    PutString("0x");
    PutString(&buf[i]);
    }

    static void PutStringdec(size_t num) {
        int i;
        char buf[21] = {0};

        if (!num) {
            PutCharacter('0');
            return;
        }

        for (i = 19; num; i--) {
            buf[i] = (num % 10) + 0x30;
            num = num / 10;
        }

        i++;
        PutString(buf + i);
    }

    void Print(const char *Format, ...) {
        va_list argp;
        va_start(argp, format);

        while (*Format != '\0') {
            if (*Format == '%') {
                Format++;
                if (*Format == 'x') {
                    PutStringhex(va_arg(argp, size_t));
                } else if (*Format == 'd') {
                    PutStringdec(va_arg(argp, size_t));
                } else if (*Format == 's') {
                PutString(va_arg(argp, char*));
                }
            } else {
                PutCharacter(*Format);
            }
            Format++;
        }

        PutCharacter('\n');
        va_end(argp);
    }
    void Info(const char Message) {
        Print("[INFO] %d", Message);
    }

    void Info(const char Message) {
        Print("[WARNING] %d", Message);
    }

    void Error(const char Message) {
        Print("[ERROR] %d", Message);
    }
}