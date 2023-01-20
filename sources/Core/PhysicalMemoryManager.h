#pragma once

namespace Neon {
    namespace MemoryManagement {
        namespace PhysicalMemoryManager {
            template<typename Type = void*>
            Type Allocate(size_t Count = 1) {
                return reinterpret_cast<Type>(Allocate(Count));
            }
            void *Allocate(size_t Count);
            void Free(void *Pointer, size_t Count);
            void Initialize();
        }
    }
}
