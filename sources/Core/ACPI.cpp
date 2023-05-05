#include <Core/ACPI.h>
#include <Common/CString.h>
#include <Core/Panic.h>
#include <cstdint>

namespace Neon {
namespace ACPI {
struct FADTHeader *FADTHeader;
struct MADTHeader *MADTHeader;

const RSDP *FindRSDP() {
	for(uint32_t i = 0xE0000; i < 0xFFFF; i += 0x10) {
		const RSDP *Result = reinterpret_cast<const RSDP *>(i);
		if(!(memcmp(Result->Signature, "RSD PTR ", 8))) {
			uint8_t Sum = 0;
			for(uint32_t j = 0; j < sizeof(*Result); j++) Sum += reinterpret_cast<const uint8_t *>(Result)[j];
			if(!Sum) return Result;
		}
	}
	return nullptr;
}

const MCFGHeader *FindMCFG(const RSDP *Rsdp) {
	const uint32_t RsdtAddress = Rsdp->RsdtAddress;
	const uint32_t RsdtEntries = (RsdtAddress + sizeof(SDTHeader));
	for(uint32_t i = sizeof(SDTHeader); i < RsdtEntries; i += 4) {
        const uint32_t EntryAddress = *reinterpret_cast<const uint32_t *>(RsdtAddress + i);
		auto *EntryHeader = reinterpret_cast<const SDTHeader *>(EntryAddress);
		if(!(memcmp(EntryHeader->Signature, "MCFG", 4))) {
			uint8_t Sum = 0;
			for(uint32_t j = 0; j < EntryHeader->Length; j++) Sum += reinterpret_cast<const uint8_t *>(EntryHeader)[j];
			if(!Sum) return reinterpret_cast<const MCFGHeader *>(EntryHeader);
		}
	}
	return nullptr;
}

const SDTHeader* FindTable(const char *Signature) {
	const RSDP* Rsdp = FindRSDP();
	if (!Rsdp) return nullptr;
	const SDTHeader* Table = nullptr;
	const SDTHeader* Rsdt = reinterpret_cast<const SDTHeader*>(rsdp->RSDTAddress);
	const uint32_t num_entries = (Rsdt->Length - sizeof(SDTHeader)) / 4;
	for(uint32_t i = 0; i < num_entries; i++) {
		const SDTHeader* Entry =
		    reinterpret_cast<const SDTHeader*>(*reinterpret_cast<const uint32_t*>(reinterpret_cast<uintptr_t>(Rsdt)
		                                                                            + sizeof(SDTHeader) + i * 4));
		if(!(memcmp(Entry->Signature, Signature, 4))) {
			Table = entry;
			break;
		}
	}
	if(!Table && Rsdp->Revision >= 2 && Rsdp->XSDTAddress) {
		const SDTHeader* Xsdt = reinterpret_cast<const SDTHeader*>(Rsdp->XSDTAddress);
		const uint32_t Entries = (Xsdt->Length - sizeof(SDTHeader)) / 8;
		for(uint32_t i = 0; i < Entries; i++) {
			const SDTHeader* Entry = reinterpret_cast<const SDTHeader*>(*reinterpret_cast<const uint64_t*>(reinterpret_cast<uintptr_t>(Xsdt) + sizeof(SDTHeader) + i * 8));
			if(!(memcmp(Entry->Signature, Signature, 4)) {
				Table = Entry;
				break;
			}
		}
	}
	return Table;
}

void Initialize() {
	const RSDP* Rsdp = FindRSDP();
	if(!Rsdp) Panic("ACPI not found!");
	const FADTHeader* Fadt = reinterpret_cast<const FADTHeader*>(Rsdp->RsdtAddress);
	if (Fadt->Header.Revision >= 2 && Rsdp->XSDTAddress)
		Fadt = reinterpret_cast<const FADTHeader*>(Rsdp->XSDTAddress);
	if(memcmp(fadt->Header.Signature, "FACP", 4) != 0)
		Panic("ACPI not found!");
	uint8_t Sum = 0;
	for(size_t i = 0; i < Fadt->Header.Length; i++)
		Sum += reinterpret_cast<const uint8_t*>(Fadt)[i];
	if(Sum != 0) Panic("ACPI not found!");
	FADTHeader = const_cast<struct FADTHeader*>(Fadt);
	const MADTHeader* Madt = reinterpret_cast<const MADTHeader*>(FindTable("APIC"));
	if (!madt) Panic("MADT not found!");
	Sum = 0;
	for(size_t i = 0; i < Madt->Header.Length; i++)
		Sum += reinterpret_cast<const uint8_t*>(Madt)[i];
	if (Sum != 0) Panic("ACPI not found!");
	MADTHeader = const_cast<struct MADTHeader*>(Madt);
}
} // namespace ACPI
} // namespace Neon
