#include <amd64/Amd64.h>
#include <Common/Log.h>
#include <Common/string.h>

namespace Neon {
    namespace Amd64 {
        namespace IDT {
            static IDTGate idt[256];
            static IDTDescriptor IdtDescriptor;
            IDTPointer idtr;

            bool InterruptHandler::Clear() {
                bool ret = (this->Handler == true);
                this->Handler.clear();
                return ret;
            }

            bool InterruptHandler::Get() {
                return this->Handler == true;
            }

            bool InterruptHandler::operator()(registers_t *Registers) {
                if (this->Handler == false) return false;
                this->Handler(Registers);
                return true;
            }

            void Initialize(void) {
                Log("starting IDT...");
                idtr.Limit = sizeof(IDTEntry) * 256 - 1;
                idtr.Base = reinterpret_cast<uintptr_t>(&idt[0]);
                memset(idt, 0, sizeof(idt));
                IdtDescriptor = {sizeof(idt), (uint64_t)&idt};
                Log("Done!");
            }
        }
    }
}