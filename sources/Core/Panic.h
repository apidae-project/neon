#pragma once

namespace Neon {
    [[noreturn]] void Panic(const char *File, int Line, const char *Function, const char *Message);
    [[noreturn]] void Panic(const char* Message);
    extern "C" [[noreturn]] void Abort() noexcept;
    void Assert(const char *Message, const char *File, int Line, const char *Function);
}