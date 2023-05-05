#include <Amd64/Amd64.h>
#include <Common/Common.h>
#include <Common/Lock.h>
#include <Common/Log.h>
#include <Core/PhysicalMemoryManager.h>
#include <LimineRequests.h>

namespace Neon {
namespace Amd64 {
namespace GDT {
TSS *Tss = nullptr;
static lock_t Lock;

void Initialize(size_t Processor) {
	Lock.Lock();
	if(Tss == nullptr) {
		Log("[GDT] Initializing...");
		Tss = new TSS[SMPRequest.response->cpu_count];
	}
	auto Base = reinterpret_cast<uintptr_t>(&tss[Processor]);
	uint16_t Limit = sizeof(tss[Processor]);
	GDT *Gdt = new GDT{{0x0000, 0, 0, 0x00, 0x00, 0},
	                   {0x0000, 0, 0, 0x9A, 0x20, 0},
	                   {0x0000, 0, 0, 0x92, 0x00, 0},
	                   {0x0000, 0, 0, 0xF2, 0x00, 0},
	                   {0x0000, 0, 0, 0xFA, 0x20, 0},
	                   {Limit,
	                    static_cast<uint16_t>(Base),
	                    static_cast<uint8_t>(Base >> 16),
	                    0x89,
	                    0x00,
	                    static_cast<uint8_t>(Base >> 24),
	                    static_cast<uint32_t>(Base >> 32),
	                    0x00}};

	GDTR Gdtr{sizeof(GDT) - 1, reinterpret_cast<uintptr_t>(Gdt)};

	asm volatile("lgdt %[Gdtr]\n\t"
	             "mov %[dsel], %%ds \n\t"
	             "mov %[dsel], %%fs \n\t"
	             "mov %[dsel], %%gs \n\t"
	             "mov %[dsel], %%es \n\t"
	             "mov %[dsel], %%ss \n\t"
	             "push %[csel] \n\t"
	             "lea 1f(%%rip), %%rax \n\t"
	             "push %%rax \n\t"
	             ".byte 0x48, 0xCB \n\t"
	             "1:" ::[Gdtr] "m"(Gdtr),
	             [dsel] "m"(GDT_DATA),
	             [csel] "i"(GDT_CODE)
	             : "rax", "memory");
	asm volatile("ltr %0" ::"r"(static_cast<uint16_t>(GDT_TSS)));
	Log("[GDT] Done!");
}
} // namespace GDT
} // namespace Amd64
} // namespace Neon