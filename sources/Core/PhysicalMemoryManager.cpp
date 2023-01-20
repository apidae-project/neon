#include <Common/Lock.h>
#include <Common/Log.h>
#include <Common/String.h>
#include <Common/Common.h>
#include <Common/Bitmap.h>
#include <Core/PhysicalMemoryManager.h>
#include <Core/Panic.h>

namespace Neon {
    namespace MemoryManagement {
        namespace PhysicalMemoryManager {
            static uintptr_t UsableMemoryTop = 0;
            uintptr_t MemoryTop = 0;
			static constexpr size_t PageSize = 0x1000;
            static lock_t *Lock;
            static size_t TotalRAM = 0;
            static size_t UsableRAM = 0;
			static bitmap_t Bitmap;
            static size_t LastIndex = 0;
            static size_t UsedRAM = 0;

            size_t TotalMemory() {
                return TotalRAM;
            }

            size_t UsableMemory() {
                return UsableRAM;
            }

            size_t UsedMemory() {
                return UsedRAM;
            }

            size_t FreeMemory() {
                return UsableRAM - UsedRAM;
            }

            void *AllocateHelper(size_t Count, size_t Limit) {
                size_t p = 0;
                while(LastIndex < Limit) {
                    if(!Bitmap[LastIndex++]) {
                        if(p++ == Count) {
                            size_t Page = LastIndex - Count;
                            for(size_t i = Page; i < LastIndex; i++) Bitmap[i] = true;
                            return reinterpret_cast<void*>(Page * PageSize);
                        }
                    }
                    else p = 0;
                }
                return nullptr;
            }

            void *Allocate(size_t Count) {
                if(Count == 0) return nullptr;
                void *ret = AllocateHelper(Count, UsableMemoryTop / PageSize);
                memset(ToHigherHalf(ret), 0, Count * PageSize);
                size_t  i = LastIndex;
                if(ret == nullptr) {
                    LastIndex = 0;
                    ret = AllocateHelper(Count, i);
                    if(ret == nullptr) Panic("Out of memory!");
                }
                UsedRAM += Count * PageSize;
                return ret;
            }

            void Free(void *Pointer, size_t Count) {
				if(Pointer == nullptr) return;
                Lock->Lock();
                size_t Page = reinterpret_cast<size_t>(Pointer) / PageSize;
                for(size_t i = Page; i < Page; i++) Bitmap[i] = false;
                UsedRAM -= Count * PageSize;
            }

            void Initialize() {
                Log("[PMM] Initializing...");
                limine_memmap_entry **MemoryMaps = MemoryMapRequest.response->entries;
                size_t MemoryMapCount = MemoryMapRequest.response->entry_count;

                for(size_t i = 0; i < MemoryMapCount; i++) {
                    uintptr_t Top = MemoryMaps[i]->base + MemoryMaps[i]->length;

                    MemoryTop = std::max(MemoryTop, Top);

                    switch(MemoryMaps[i]->type) {
                        case LIMINE_MEMMAP_USABLE:
                            UsableRAM += MemoryMaps[i]->length;
                            UsableMemoryTop = std::max(UsableMemoryTop, Top);
                        [[fallthrough]];
                        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                        case LIMINE_MEMMAP_KERNEL_AND_MODULES:
                            TotalRAM += MemoryMaps[i]->length;
                            break;
                    }
                }

                size_t BitmapSize = AlignUp((UsableMemoryTop / 0x1000) / 8, 0x1000);

                for(size_t i = 0; i < MemoryMapCount; i++) {
                    if(MemoryMaps[i]->type != LIMINE_MEMMAP_USABLE) continue;

                    if(MemoryMaps[i]->length >= BitmapSize) {
                        Bitmap.Buffer = reinterpret_cast<uint8_t*>(ToHigherHalf(MemoryMaps[i]->base));
                        Bitmap.Size = BitmapSize;
                        memset(Bitmap.Buffer, 0xFF, Bitmap.Size);
                        MemoryMaps[i]->length -= BitmapSize;
                        MemoryMaps[i]->base += BitmapSize;
                        UsedRAM += BitmapSize;
                        break;
                    }
                }

                for(size_t i = 0; i < MemoryMapCount; i++) {
                    if (MemoryMaps[i]->type != LIMINE_MEMMAP_USABLE) continue;
                    for (uintptr_t t = 0; t < MemoryMaps[i]->length; t += 0x1000) Bitmap[(MemoryMaps[i]->base + t) / 0x1000] = false;
                }

                MemoryTop = AlignUp(MemoryTop, 0x40000000);
                Log("[PMM] Done!");
            }
        }
    }
}