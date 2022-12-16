#include <Core/Panic.h>
#include <Common/Log.h>

namespace Neon {
    [[noreturn]] void Panic(const char *File, int Line, const char *Function, const char *Message) {
        Error("======= Panic =======");
        Error(Message);
        Error("File {} at line {} in function {}.", File, Line, Function);
    }

    [[noreturn]] void Panic(const char* Message) {
        Error("======= Panic =======");
        Error(Message);
        Error("CPU has been halted.");
        Error("Goodbye, world.");
        asm("cli\n hlt");
    }

    [[noreturn]] void Abort() noexcept {
        Panic("Abort() called.");
    }
}