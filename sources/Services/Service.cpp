#include <Services/Service.h>
#include <Core/ELF.h>
#include <cstddef>

namespace Neon {
    namespace Services {
        void Load(const char *Path, size_t PathLength) {
            ELF::Load(0, 0);
        }

        void Unload(const char *Name, size_t NameLength) {

        }
    }
}