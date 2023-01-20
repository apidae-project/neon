#include <Core/Panic.h>
#include <Common/Log.h>

namespace Neon {
    [[noreturn]] void Panic(const char *File, int Line, const char *Function, const char *Message) {
        Error("======= Panic =======");
        Error(Message);
        Error("At file %s at line %d in function %s.", File, Line, Function);
    }

    [[noreturn]] void Panic(const char* Message) {
        Error("======= Panic =======");
        Error(Message);
        Error("CPU has been halted.");
        Error("Goodbye, world.");
        asm("cli\n hlt");
    }

    extern "C" void AssertionFail(const char *Message, const char *File, int Line, const char *Function) {
        Panic(File, Line, Function, Message);
    }

    extern "C" [[noreturn]] void abort() noexcept {
        Panic("Abort() called.");
    }
}