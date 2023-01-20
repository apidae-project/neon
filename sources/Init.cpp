#include <Amd64/Amd64.h>
#include <Core/PhysicalMemoryManager.h>
#include <Devices/Framebuffer.h>
#include <Neon.h>
#include <Core/Panic.h>
#include <Common/Serial.h>
#include <Common/Log.h>
#include <LimineRequests.h>

using namespace Neon;
using namespace Neon::Amd64;

extern "C" void Main(void) {
    if (TerminalRequest.response == nullptr || TerminalRequest.response->terminal_count < 1) asm("hlt");
    Log("\e[1;1H\e[2J");
    Log("Neon v0.1-rc1\n");
    #if defined(TEST_MODE)
    Warn("Look i'm a warning!");
    Error("Aaaaaaand i'm an error!");
    #endif
    EnableCPUFeatures();
    Serial::Initialize();
    GDT::Initialize();
    IDT::Initialize();
    MemoryManagement::PhysicalMemoryManager::Initialize();
    VirtualMemoryManager::Initialize();
    #ifdef TEST_MODE
    Log("Running PMM tests");
    void *e = MemoryManagement::PhysicalMemoryManager::Allocate(69);
    Log("%d", e);
    MemoryManagement::PhysicalMemoryManager::Free(e, sizeof(e));
    #endif
    #if defined(TEST_MODE)
    Panic("Haha yes test kernel panic.");
    #endif
}