#include <Amd64/Amd64.h>
#include <Common/Log.h>
#include <Common/Serial.h>
#include <Core/Panic.h>
#include <Core/PhysicalMemoryManager.h>
#include <Devices/Framebuffer.h>
#include <LimineRequests.h>
#include <Neon.h>

using namespace Neon;
using namespace Neon::Amd64;

extern "C" void Main(void) {
	if(!FrameBufferRequest.response
	    || FrameBufferRequest.response->framebuffer_count < 0) asm("cli; hlt");
	Log("\e[1;1H\e[2J");
	Log("Neon v0.1-rc1\n");
#ifdef TEST_MODE
	Warn("Look i'm a warning!");
	Error("Aaaaaaand i'm an error!");
#endif
	EnableCPUFeatures();
	Serial::Initialize();
	GDT::Initialize();
	IDT::Initialize();
	PhysicalMemoryManager::Initialize();
	VirtualMemoryManager::Initialize();
	Devices::Framebuffer::Initialize();
#ifdef TEST_MODE
	Panic("Haha yes test kernel panic.");
#endif
}