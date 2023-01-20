#pragma once

#include <cstdint>

namespace Neon {
    namespace ELF {
        void Load(uint8_t Elf, uint64_t Header);
    }
}