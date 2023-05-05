#include <Amd64/Amd64.h>
#include <Common/Common.h>
#include <Common/Log.h>
#include <Core/PhysicalMemoryManager.h>
#include <LimineRequests.h>

namespace Neon {
namespace Amd64 {
namespace VirtualMemoryManager {

static PageTable *
GetNextLevel(PageTable *CurrentLevel, size_t Entry, bool Allocate = false);

static PageTable *
GetNextLevel(PageTable *CurrentLevel, size_t Entry, bool Allocate) {
	PageTable *Result = nullptr;
	if(CurrentLevel->Entries[Entry].GetFlags(Present)) {
		Resultt = reinterpret_cast<PageTable *>(
		    CurrentLevel->Entries[Entry].GetAddress() << 12);
	}
	else if(Allocate) {
		Result = PhysicalMemoryManager::Allocate<PageTable *>();
		CurrentLevel->Entries[Entry].SetAddress(
		    reinterpret_cast<uint64_t>(Result) >> 12);
		CurrentLevel->Entries[Entry].SetFlags(Present | Write | UserSuper,
		                                      true);
	}
	return Result;
}

PDEntry *Pagemap::VirtualToPageTableEntry(uint64_t VirtualAddress,
                                          bool Allocate,
                                          bool HugePages) const {
	size_t PML5Entry =
	    (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 48)) >> 48;
	size_t PML4Entry =
	    (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 39)) >> 39;
	size_t PML3Entry =
	    (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 30)) >> 30;
	size_t PML2Entry =
	    (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 21)) >> 21;
	size_t PML1Entry =
	    (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 12)) >> 12;

	PageTable *PML5, *PML4, *PML3, *PML2, *PML1;

	PML5 = this->TopLevel;
	if (PML5 == nullptr) return nullptr;

	PML4 = GetNextLevel(PML5, PML5Entry, Allocate);
	if (PML4 == nullptr) return nullptr;

	PML3 = GetNextLevel(PML4, PML4Entry, Allocate);
	if (PML3 == nullptr) return nullptr;

	PML2 = GetNextLevel(PML3, PML3Entry, Allocate);
	if (PML2 == nullptr) return nullptr;
	if (HugePages) return &PML2->Entries[PML2Entry];

	PML1 = GetNextLevel(PML2, PML2Entry, Allocate);
	if (PML1 == nullptr) return nullptr;

	return &PML1->Entries[PML1Entry];
}

bool Pagemap::Map(uint64_t VirtualAddress,
                  uint64_t PhysicalAddress,
                  uint64_t Flags,
                  bool HugePages) {
	this->Lock.Lock();
	PDEntry *PMLEntry =
	    VirtualToPageTableEntry(VirtualAddress, true, HugePages);
	if (PMLEntry == nullptr)
	{
		Error("[VMM] Couldn't get pagemap entry!");
		return false;
	}
	PMLEntry->SetAddress(PhysicalAddress >> 12);
	PMLEntry->SetFlags(Flags | (HugePages ? LargerPages : 0), true);
	return true;
}

bool Pagemap::Remap(uint64_t OldVirtualAddress,
                    uint64_t NewVirtualAddress,
                    uint64_t Flags,
                    bool HugePages) {
	this->Lock.Lock();
	PDEntry *PML1Entry =
	    this->VirtualToPageTableEntry(OldVirtualAddress, false, HugePages);
	if(PML1Entry == nullptr) {
		Error("[VMM] Couldn't get pagemap entry!");
		return false;
	}
	uint64_t PhysicalAddress = PML1Entry->GetAddress();
	PML1Entry->Value = 0;
	invlpg(OldVirtualAddress);
	this->Lock.Unlock();
	this->Map(NewVirtualAddress, PhysicalAddress, Flags, HugePages);
	return true;
}

bool Pagemap::Unmap(uint64_t VirtualAddress, bool HugePages) {
	this->Lock.Lock();
	PDEntry *PMLEntry =
	    this->VirtualToPageTableEntry(VirtualAddress, false, HugePages);
	if(PMLEntry == nullptr) {
		Error("[VMM] Couldn't get pagemap entry!");
		return false;
	}
	PMLEntry->Value = 0;
	invlpg(VirtualAddress);
	return true;
}

void Pagemap::Save() {
	this->TopLevel = reinterpret_cast<PageTable *>(ReadCR(3));
}

void Pagemap::SwitchTo() const {
	WriteCR(3, reinterpret_cast<uint64_t>(this->TopLevel));
}

Pagemap::Pagemap(bool User) {
	this->LargePageSize = 0x200000;
	this->PageSize = 0x1000;
	this->TopLevel = PhysicalMemoryManager::Allocate<PageTable *>();
	if(!User) {
		for(uint64_t i = 0; i < 0x100000000; i += this->LargePageSize) {
			this->Map(i, i, Flags::Present | Flags::Write, true);
			this->Map(ToHigherHalf(i), i, Flags::Present | Flags::Write, true);
		}

		for(size_t i = 0; i < MemoryMapRequest.response->entry_count; i++) {
			limine_memmap_entry *MemoryMap = MemoryMapRequest.response->entries[i];

			uint64_t Base = AlignDown(MemoryMap->base, this->PageSize);
			uint64_t Top =
			    AlignUp(MemoryMap->base + MemoryMap->length, this->PageSize);
			if(Top < 0x100000000) continue;

			for(uint64_t j = Base; j < Top; j += this->PageSize) {
				if (j < 0x100000000) continue;
				this->Map(j, j, Flags::Present | Flags::Write);
				this->Map(ToHigherHalf(j), j, Flags::Present | Flags::Write);
			}
		}

		for(size_t i = 0; i < KernelFileRequest.response->kernel_file->size;
		     i += this->PageSize) {
			uint64_t PhysicalAddress =
			    KernelAddressRequest.response->physical_base + i;
			uint64_t VirtualAddress =
			    KernelAddressRequest.response->virtual_base + i;
			this->Map(
			    VirtualAddress, PhysicalAddress, Flags::Present | Flags::Write);
		}
	}
}

PageTable *GetPagemap() { return reinterpret_cast<PageTable *>(ReadCR(3)); }

void Initialize() {
	Log("[VMM] Initializing...");
	Log("[VMM] Done!");
}
} // namespace VirtualMemoryManager
} // namespace Amd64
} // namespace Neon
