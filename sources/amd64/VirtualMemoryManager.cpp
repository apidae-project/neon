#include <Common/Log.h>
#include <Amd64/Amd64.h>

namespace Neon {
    namespace Amd64 {
        PDEntry *Pagemap::VirtualToPageTableEntry(uint64_t VirtualAddress, bool Allocate, bool HugePages) {
            size_t PML5Entry = (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 48)) >> 48;
            size_t PML4Entry = (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 39)) >> 39;
            size_t PML3Entry = (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 30)) >> 30;
            size_t PML2Entry = (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 21)) >> 21;
            size_t PML1Entry = (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 12)) >> 12;

            PTable *pml5, *pml4, *pml3, *pml2, *pml1;

            pml5 = this->TopLevel;
            if (pml5 == nullptr) return nullptr;

            pml4 = GetNextLevel(pml5, PML5Entry, Allocate);
            if (pml4 == nullptr) return nullptr;

            pml3 = GetNextLevel(pml4, PML4Entry, Allocate);
            if (pml3 == nullptr) return nullptr;

            pml2 = GetNextLevel(pml3, PML3Entry, Allocate);
            if (pml2 == nullptr) return nullptr;
            if (HugePages) return &pml2->Entries[PML2Entry];

            pml1 = GetNextLevel(pml2, PML2Entry, Allocate);
            if (pml1 == nullptr) return nullptr;

            return &pml1->entries[PML1Entry];
        }

        bool Pagemap::Map(uint64_t VirtualAddress, uint64_t PhysicalAddress, uint64_t Flags, bool HugePages) {
            Lock(this->Lock);
            PDEntry *PMLEntry = VirtualToPageTableEntry(VirtualAddress, true, HugePages);
            if(PMLEntry == nullptr) { Error("[VMM] Couldn't get pagemap entry!"); return false; }
            PMLEntry->SetAddress(PhysicalAddress >> 12);
            PMLEntry->SetFlags(Flags | (HugePages ? LargerPages : 0), true);
            return true;
        }

        bool Pagemap::Remap(uint64_t OldVirtualAddress, uint64_t NewPhysicalAddress, uint64_t Flags, bool HugePages) {

        }

        bool Pagemap::Unmap(uint64_t VirtualAddress, bool HugePages) {

        }

        void Pagemap::Save() {

        }

        void Pagemap::SwitchTo() {

        }
    }
}
