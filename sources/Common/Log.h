#pragma once

#include <cstdarg>
#include <cstddef>

namespace Neon {
    #define ANSI_COLOR_RED     "\x1b[31m"
    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_ORANGE  "\x1b[33m"
    #define ANSI_COLOR_BLUE    "\x1b[34m"
    #define ANSI_COLOR_MAGENTA "\x1b[35m"
    #define ANSI_COLOR_CYAN    "\x1b[36m"
    #define ANSI_COLOR_RESET   "\x1b[0m"
    #define CLEAR "\e[1;1H\e[2J"

    void e9_putc(char c);
    void e9_print(const char *Message);
    void e9_puts(const char *Message);
    void Log(const char *Format, ...);
    void Error(const char *Message, ...);
    void Warn(const char *Message, ...);
}