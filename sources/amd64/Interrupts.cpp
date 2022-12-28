#include <amd64/Amd64.h>
#include <Neon.h>

namespace Amd64 {
    namespace Interrupts {
        static const char *ExceptionMessages[32] {
            "Division by zero",
            "Debug",
            "Non-maskable interrupt (NMI)",
            "Breakpoint",
            "Detected overflow",
            "Out-of-bounds",
            "Invalid opcode",
            "No coprocessor",
            "Double fault",
            "Coprocessor segment overrun",
            "Bad TSS",
            "Segment not present",
            "Stack fault",
            "General protection fault",
            "Page fault",
            "Unknown interrupt",
            "Coprocessor fault",
            "Alignment check",
            "Machine check",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
            "Reserved",
        };

        extern "C" void InterruptHandler(registers_t *Registers) {
            if(Registers->int_no < 32) ExceptionHandler(Regsiters);
            else Panic("Unkown interrupt!");
        }

        static void ExceptionHandler(registers_t *Registers) {
            Panic("Exception occured: %d", ExceptionMessages[Registers->int_no]);
        }

        void Mask(uint8_t Irq) {
            if(IOAPIC::Initialized) IOAPIC::Mask(Irq);
        }

        void Unmask(uint8_t Irq) {
            if(IOAPIC::Initialized) IOAPIC::Mask(Irq);
        }
    }
}