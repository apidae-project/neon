#include <Common/Lock.h>
#include <LimineRequests.h>

namespace Neon {
    namespace Memory {
        namespace PhysicalMemory {
            static lock_t *Lock;
            static size_t FreeRAM = 0;
            static size_t UsedRAM = 0;

            static size_t FreeMemory(void) {
                return FreeRAM;
            }

            static size_t UsedMemory(void) {
                return UsedRAM;
            }

            auto Allocate(size_t Count) {
                lockit(Lock);
                UsedRAM += Count;
                FreeRAM -= Count;
            }

            auto Free(size_t Count) {
                lockit(Lock);
                FreeRAM += Count;
                UsedRAM -= Count;
            }

            void Initialize(void) {
                lockit(Lock);
                MemoryMapCount = MemoryMapRequest.response->memmap_count;
            }
        }
    }
}