#pragma once

namespace Neon
{
template <typename... Args>
[[noreturn]] void Panic(const char *File,
                        const int Line,
                        const char *Function,
                        const char *Message,
                        const Args &...Arguments);
template <typename... Args>
[[noreturn]] void Panic(const char *Message, const Args &...Arguments);
extern "C" void AssertionFail(const char *Message,
                              const char *File,
                              const int Line,
                              const char *Function,
                              const Args &...Arguments);
} // namespace Neon
