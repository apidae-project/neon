#include <amd64/Amd64.h>
#include <Neon.h>
#include <Core/Panic.h>
#include <Common/Log.h>
#include <LimineRequests.h>

using namespace Neon::Amd64;

namespace Neon {
    namespace Init {
        extern "C" void Main(void) {
            uintptr_t HHDMOffset;
            struct limine_terminal *Terminal = TerminalRequest.response->terminals[0];
            if(TerminalRequest.response == NULL || TerminalRequest.response->terminal_count < 1) asm("hlt");
            HHDMOffset = HHDMRequest.response->offset;
            Log("\e[1;1H\e[2J");
            Log("Neon v0.1-rc1\n");
            #if defined(TEST_MODE)
                Warn("Look i'm a warning!");
                Error("Aaaaaaand i'm an error!");
            #endif
            EnableCPUFeatures();
            IDT::Initialize();
            TerminalRequest.response->write(Terminal, "Apidae.", 7);
            #if defined(TEST_MODE)
                Panic("Haha yes test kernel panic.");
            #endif
        }
    }
}