#include <KernelMain/LimineRequests.hxx>

extern "C" void EarlyInit() {
    if (LimineTerminalRequest.response = NULL || LimineTerminalRequest.response->terminal_count < 1) {
        asm("hlt");
    }
}

void main() {
    EarlyInit();
    struct limine_terminal *terminal = TerminalRequest.response->terminals[0];
    LimineTerminalRequest.response->Print(terminal, "Hello, World!", 13);
    for (;;) {
        asm("hlt");
    }
}