#include <Amd64/Amd64.h>
#include <LimineRequests.h>
#include <Common/Log.h>
#include <Common/Lock.h>
#include <Neon.h>

namespace Neon {
namespace Amd64 {
extern "C" Processor *ThisCPU() { return &Processors[ReadGS(0)]; }
extern "C" void *SystemCallEntry();

void InititalizeBSP(limine_smp_info *CPU) {
	auto ProcessorPointer = reinterpret_cast<Processor *>(CPU->extra_argument);
	SetKernelGS(CPU->extra_argument);
	SetGS(CPU->extra_argument);
}

void InitializeProcessor() {
	Log("[SMP] Initializing x86_64 processor...");
	wrmsr(0xC0000080, rdmsr(0xC0000080) | (1 << 0));
	wrmsr(0xC0000081,
	      ((uint64_t(GDT::GDT_DATA) | 0x03) << 48) |
	          (uint64_t(GDT::GDT_CODE) << 32));
	wrmsr(0xC0000082, reinterpret_cast<uint64_t>(SystemCallEntry()));
	wrmsr(0xC0000084, ~uint32_t(2));
	Log("[SMP] Done!");
}
} // namespace Amd64
} // namespace Neon
