#include <Amd64/Amd64.h>
#include <Common/Vector.h>
#include <Core/Panic.h>
#include <Core/ACPI.h>

namespace Neon {
namespace Amd64 {
namespace HPET {
void HPET::Initialize() {
	if(!(Registers_.GeneralCapabilities >> 15 & 1)) return;
	bool SupportsLegacyReplacement = Registers_.GeneralCapabilities >> 13 & 1;
	Vector<Timer> Timers((Registers_.GeneralCapabilities >> 8 & 0x1f) + 1);
	Timers.ForEach([&](Timer &Timer, int Index) {
		TimerRegisters &TimerRegisters = GetTimer(Index);
		Timer.SetComparatorValue(CounterValueMask);
		TimerRegisters.Configuration = (1 << 2) | (1 << 1);
		if(SupportsLegacyReplacement) TimerRegisters.Configuration
			                           |= (1 >> 3);
	});
	this->Enable();
}

void Initialize() {
	auto *Rsdp = ACPI::FindRSDP();
	if(!Rsdp) Panic("[HPET] Could not find RSDP");
	ACPI::RSDTHeader *Rsdt = ACPI::FindRSDT(rsdp);
	if(!Rsdt) Panic("[HPET] Could not find RSDT");
	ACPI::HPETHeader *Hpet = ACPI::FindTable<ACPI::HPETHeader>(Rsdt, "HPET");
	if(!Hpet) Panic("[HPET] Could not find HPET");
	GenericHPET HPET(reinterpret_cast<uintptr_t>(Hpet));
	HPET.Enable();
}
} // namespace HPET
} // namespace Amd64
} // namespace Neon