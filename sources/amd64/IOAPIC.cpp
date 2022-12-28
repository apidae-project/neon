#include <amd64/Amd64.h>
#include <Amd64/MMIO.h>
#include <Cxx/cstdint>

namespace Neon {
    namespace Amd64 {
        namespace IOAPIC {
            bool Initialized = false;

            uint32_t Read(uint32_t Register) {
                mmout<uint32_t>(MMIOBase, Register);
                return ;
            }

            uint32_t Write(uint32_t Register, uint32_t Value) {

            }

            void Mask(uint64_t Irq) {

            }

            void UnMask(uint64_t Irq) {
                
            }

            void Initialize(void) {
                Initialized = true;
            }
        }
    }
}