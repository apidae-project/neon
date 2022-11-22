#include <Common/Lock.h>

namespace Neon {
    namespace mm {
        namespace pmm {
            static lock_t *Lock;
            static size_t FreeRAM;
            static size_t UsedRAM;
        }
    }
}