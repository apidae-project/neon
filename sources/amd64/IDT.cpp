#include <Amd64/Amd64.h>
#include <Common/CString.h>
#include <Common/Log.h>
#include <Core/Panic.h>

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

bool InterruptHandler::Get() { return this->Handler == true; }

bool InterruptHandler::operator()(registers_t *Registers) {
	if (this->Handler == false) return false;
	this->Handler(Registers);
	return true;
}

static const char* ExceptionMessages[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack-segment fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "x87 FPU floating-point error",
    "Alignment check",
    "Machine check",
    "SIMD floating-point exception",
    "Virtualization exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security exception",
    "Reserved",
    "Triple fault"
};

static void ExceptionHandler(registers_t *Registers) {
	if(Registers->cs & 0x03)
		Panic("Exception {} on CPU: ", ExceptionMessages[Registers->int_no]);
}

extern "C" void InterruptHandler(registers_t *Registers) {
	if(Registers->int_no < 32) ExceptionHandler(Registers);
	else if(Registers->int_no >= 32 && Registers->int_no < 256) return;
	else Panic("Unkown interrupt {}.", Registers->int_no);
}

bool RegisterInterruptHandler(uint8_t InterruptNumber
                              , InterruptHandler &Handler) {
	if(InterruptNumber > 255) {
		Log("[IDT] Invalid interrupt number: {}", InterruptNumber);
		return false;
	}
	Idt[InterruptNumber].Set(InterruptHandler);
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
} // namespace IDT
} // namespace Amd64
} // namespace Neon
