#pragma once

namespace Neon {
    [[noreturn]] void Panic(const char *File, int Line, const char *Function, const char *Message);
    [[noreturn]] void Panic(const char* Message);
    extern "C" void AssertionFail(const char *Message, const char *File, int Line, const char *Function);
    extern "C" [[noreturn]] void abort() noexcept;
}