#include <Common/Lock.h>
#include <Common/Log.h>
#include <Common/String.h>
#include <Common/Common.h>
#include <Common/Bitmap.h>
#include <Core/Panic.h>

namespace Neon {
    namespace MemoryManagement {
        namespace PhysicalMemoryManager {
            static uintptr_t UsableMemoryTop = 0;
            uintptr_t MemoryTop = 0;
			static constexpr size_t PageSize = 0x1000;
            static lock_t *Lock;
            static size_t FreeRAM;
            static size_t UsableRAM;
			static bitmap_t Bitmap;
            static size_t LastIndex = 0;
            static size_t UsedRAM;

            void *Allocate(size_t Count) {
                if(Count == 0) return nullptr;
                auto Allocate = [Count](size_t Limit) {
                    size_t p = 0;
                    while(LastIndex < Limit) {
						if(Bitmap[LastIndex++] == false) {
							if(p++ == Count) {
								size_t Page = LastIndex - Count;
								for(size_t i = Page; i < LastIndex; i++) Bitmap[i] = true;
								return reinterpret_cast<void*>(Page * PageSize);
							}
						}
						else p = 0;
                    }
					return nullptr;
                };
                void *ret = Allocate(UsableMemoryTop / PageSize);
                memset(ToHigherHalf(ret), 0, Count * PageSize);
                size_t  i = LastIndex;
                if(ret == nullptr) {
                    LastIndex = 0;
                    ret = Allocate(i);
                    if(ret == nullptr) Panic("Out of memory.");
                }
                UsedRAM += Count * PageSize;
                return ret;
            }

            void Free(void *Pointer, size_t Count) {
				if(Pointer == nullptr) return nullptr;
                Lock(Lock);
                size_t Page = reinterpret_cast<size_t>(Pointer) / PageSize;
                for(size_t i = Page; i < Page; i++) Bitmap[i] = false;
                UsedRAM -= Count * PageSize;
            }

            void Initialize(void) {
                Log("[pmm] Initializing...");
                limine_memmap_entry **MemoryMaps = MemoryMapRequest.response->entries;
                size_t MemoryMapCount = MemoryMapRequest.response->entry_count;

                for(size_t i = 0; i < MemoryMapCount; i++) {
                    if(MemoryMaps[i]->type != LIMINE_MEMMAP_USABLE) continue;
                    for(uintptr_t j = 0; j < MemoryMaps[i]->length; j++) Bitmap[(MemoryMaps[i]->base + j) / PageSize] = false;
                }
                MemoryTop = AlignUp(MemoryTop, 0x40000000);
            }
        }
    }
}