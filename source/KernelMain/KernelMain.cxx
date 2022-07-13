#include <KernelMain/LimineRequests.hpp>

extern "C" void EarlyInit() {
    if (LimineTerminalRequest.response = NULL || LimineTerminalRequest.response->terminal_count < 1) {
        asm("hlt");
    }
}

void main() {
    EarlyInit();
    LimineTerminalRequest.response->write(terminal, "Hello, World!", 13);
}