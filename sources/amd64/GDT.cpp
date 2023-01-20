#include <Common/Common.h>
#include <Core/PhysicalMemoryManager.h>
#include <Amd64/Amd64.h>
#include <Common/Log.h>
#include <Common/Lock.h>

namespace Neon {
    namespace Amd64 {
        namespace GDT {
            TSS *tss = nullptr;
            static lock_t Lock;

            void Initialize() {
                Lock.Lock();
                Log("[GDT] Initializing...");
                Log("[GDT] Done!");
            }
        }
    }
}