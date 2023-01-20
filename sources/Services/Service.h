#pragma once

#include <cstddef>

namespace Neon {
    namespace Services {
        void Load(const char *Path, size_t PathLength);
        void Unload(const char *Name, size_t NameLength);
    }
}