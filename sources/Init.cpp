#include <amd64/amd64.h>
#include <Common/Log.h>
#include <LimineRequests.h>

using namespace Neon::Amd64;

namespace Neon {
    namespace Init {
        extern "C" void Main(void) {
            if (TerminalRequest.response == NULL || TerminalRequest.response->terminal_count < 1) {
                asm("hlt");
            }
            struct limine_terminal *Terminal = TerminalRequest.response->terminals[0];
            Log("Neon v0.1-rc1\n");
            EnableCPUFeatures();
            TerminalRequest.response->write(Terminal, "apidae", 6);
        }
    }
}