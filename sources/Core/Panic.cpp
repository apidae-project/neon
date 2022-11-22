#include <Core/Panic.h>
#include <Common/Log.h>

namespace Neon {
    [[noreturn]] void panic(const char* Message) {
        Error("======= PANIC =======\n");
        Error("%d", Message);
        Error("CPU has been halted.");
        Error("Goodbye, world.");
        asm("cli\n hlt");
    }
}