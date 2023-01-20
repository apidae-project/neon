#include <Common/Log.h>
#include <Common/Common.h>
#include <Core/PhysicalMemoryManager.h>
#include <LimineRequests.h>
#include <Amd64/Amd64.h>

namespace Neon {
    namespace Amd64 {
        namespace VirtualMemoryManager {
            Pagemap *KernelPagemap = nullptr;

            static PageTable *GetNextLevel(PageTable *CurrentLevel, size_t Entry, bool Allocate) {
                PageTable *ret = nullptr;
                if(CurrentLevel->Entries[Entry].GetFlags(Present)) {
                    ret = reinterpret_cast<PageTable*>(CurrentLevel->Entries[Entry].GetAddress() << 12);
                } else if(Allocate) {
                    ret = MemoryManagement::PhysicalMemoryManager::Allocate<PageTable*>();
                    CurrentLevel->Entries[Entry].SetAddress(reinterpret_cast<uint64_t>(ret) >> 12);
                    CurrentLevel->Entries[Entry].SetFlags(Present | Write | UserSuper, true);
                }
                return ret;
            }

            PDEntry *Pagemap::VirtualToPageTableEntry(uint64_t VirtualAddress, bool Allocate, bool HugePages) {
                size_t PML5Entry = (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 48)) >> 48;
                size_t PML4Entry = (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 39)) >> 39;
                size_t PML3Entry = (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 30)) >> 30;
                size_t PML2Entry = (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 21)) >> 21;
                size_t PML1Entry = (VirtualAddress & (static_cast<uint64_t>(0x1FF) << 12)) >> 12;

                PageTable *pml5, *pml4, *pml3, *pml2, *pml1;

                pml5 = this->TopLevel;
                if(pml5 == nullptr) return nullptr;

                pml4 = GetNextLevel(pml5, PML5Entry, Allocate);
                if(pml4 == nullptr) return nullptr;

                pml3 = GetNextLevel(pml4, PML4Entry, Allocate);
                if(pml3 == nullptr) return nullptr;

                pml2 = GetNextLevel(pml3, PML3Entry, Allocate);
                if(pml2 == nullptr) return nullptr;
                if(HugePages) return &pml2->Entries[PML2Entry];

                pml1 = GetNextLevel(pml2, PML2Entry, Allocate);
                if(pml1 == nullptr) return nullptr;

                return &pml1->Entries[PML1Entry];
            }

            bool Pagemap::Map(uint64_t VirtualAddress, uint64_t PhysicalAddress, uint64_t Flags, bool HugePages) {
                this->Lock.Lock();
                PDEntry *PMLEntry = VirtualToPageTableEntry(VirtualAddress, true, HugePages);
                if (PMLEntry == nullptr) {
                    Error("[VMM] Couldn't get pagemap entry!");
                    return false;
                }
                PMLEntry->SetAddress(PhysicalAddress >> 12);
                PMLEntry->SetFlags(Flags | (HugePages ? LargerPages : 0), true);
                return true;
            }

            bool Pagemap::Remap(uint64_t OldVirtualAddress, uint64_t NewVirtualAddress, uint64_t Flags, bool HugePages) {
                this->Lock.Lock();
                PDEntry *PML1Entry = this->VirtualToPageTableEntry(OldVirtualAddress, false, HugePages);
                if (PML1Entry == nullptr) {
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
                PDEntry *PMLEntry = this->VirtualToPageTableEntry(VirtualAddress, false, HugePages);
                if (PMLEntry == nullptr) {
                    Error("[VMM] Couldn't get pagemap entry!");
                    return false;
                }
                PMLEntry->Value = 0;
                invlpg(VirtualAddress);
                return true;
            }

            void Pagemap::Save() {
                this->TopLevel = reinterpret_cast<PageTable*>(ReadCR(3));
            }

            void Pagemap::SwitchTo() {
                WriteCR(3, reinterpret_cast<uint64_t>(this->TopLevel));
            }

            Pagemap::Pagemap(bool User) {
                this->LargePageSize = 0x200000;
                this->PageSize = 0x1000;
                this->TopLevel = MemoryManagement::PhysicalMemoryManager::Allocate<PageTable*>();
                if (!User) {
                    for (uint64_t i = 0; i < 0x100000000; i += this->LargePageSize) {
                        this->Map(i, i, Flags::Present | Flags::Write, true);
                        this->Map(ToHigherHalf(i), i, Flags::Present | Flags::Write, true);
                    }

                    for (size_t i = 0; i < MemoryMapRequest.response->entry_count; i++) {
                        limine_memmap_entry *MemoryMap = MemoryMapRequest.response->entries[i];

                        uint64_t Base = AlignDown(MemoryMap->base, this->PageSize);
                        uint64_t Top = AlignUp(MemoryMap->base + MemoryMap->length, this->PageSize);
                        if (Top < 0x100000000) continue;

                        for (uint64_t j = Base; j < Top; j += this->PageSize) {
                            if(j < 0x100000000) continue;
                            this->Map(j, j, Flags::Present | Flags::Write);
                            this->Map(ToHigherHalf(j), j, Flags::Present | Flags::Write);
                        }
                    }

                    for (size_t i = 0; i < KernelFileRequest.response->kernel_file->size; i += this->PageSize) {
                        uint64_t PhysicalAddress = KernelAddressRequest.response->physical_base + i;
                        uint64_t VirtualAddress = KernelAddressRequest.response->virtual_base + i;
                        this->Map(VirtualAddress, PhysicalAddress, Flags::Present | Flags::Write);
                    }
                }
            }

            PageTable *GetPagemap() {
                return reinterpret_cast<PageTable *>(ReadCR(3));
            }

            void Initialize() {
                Log("[VMM] Initializing...");
                KernelPagemap = new Pagemap(false);
                KernelPagemap->SwitchTo();
                Log("[VMM] Done!");
            }
        }
    }
}
