#include <Amd64/Amd64.h>
#include <Common/Log.h>
#include <Common/String.h>

namespace Neon {
    namespace Amd64 {
        namespace IDT {
            static IDTGate Idt[256];
            static IDTDescriptor IdtDescriptor;
            IDTPointer idtr;

            bool InterruptHandler::Clear() {
                this->Handler.clear();
                return this->Handler == true;
            }

            bool InterruptHandler::Get() {
                return this->Handler == true;
            }

            bool InterruptHandler::operator()(registers_t *Registers) {
                if (this->Handler == false) return false;
                this->Handler(Registers);
                return true;
            }

            void Initialize() {
                Log("[IDT] Initializing");
                idtr.Limit = sizeof(IDTEntry) * 256 - 1;
                idtr.Base = reinterpret_cast<uintptr_t>(&Idt[0]);
                memset(Idt, 0, sizeof(Idt));
                IdtDescriptor = {sizeof(Idt), (uint64_t)&Idt};
                Log("[IDT] Done!");
            }
        }
    }
}