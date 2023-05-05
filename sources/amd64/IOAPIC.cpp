#include <Amd64/Amd64.h>
#include <Common/Common.h>
#include <Common/Log.h>
#include <Common/Vector.h>
#include <cstdint>

namespace Neon {
namespace Amd64 {
namespace IOAPIC {
Vector<IOAPIC> IOAPICs;
bool Initialized = false;
uint32_t IOAPIC::Read(uint32_t Register) {
	mmout<uint32_t>(this->MMIOBase, Register);
	return mmin<uint32_t>(this->MMIOBase + 0x10);
}

void IOAPIC::Write(uint32_t Register, uint32_t Value) {
	mmout<uint32_t>(this->MMIOBase, Register);
	mmout<uint32_t>(this->MMIOBase + 0x10, Value);
}

uint32_t IOAPIC::ReadEntry(uint32_t Entry) {
	return this->Read(
	    this->Entry(Entry) |
	    (static_cast<uint64_t>(this->Read(this->Entry(Entry) + 0x01)) << 32));
}

void IOAPIC::WriteEntry(uint32_t Entry, uint64_t Value) {
	this->Write(this->Entry(Entry), static_cast<uint32_t>(Value));
	this->Write(this->Entry(Entry) + 0x01, static_cast<uint32_t>(Value >> 32));
}

void IOAPIC::Mask(uint8_t InterruptRequest) {
	this->WriteEntry(InterruptRequest,
	                 this->ReadEntry(InterruptRequest) | (1 << 16));
}

void IOAPIC::Unmask(uint8_t InterruptRequest) {
	this->WriteEntry(InterruptRequest,
	                 this->ReadEntry(InterruptRequest) & ~(1 << 16));
}

void IOAPIC::Set(uint8_t i,
                 uint8_t Vector,
                 DeliveryMode Delivery,
                 DestinationMode Destination,
                 uint16_t Flags,
                 uint8_t id) {
	uint64_t Value = 0;
	Value |= Vector;
	Value |= AsInteger(Delivery) << 8;
	Value |= AsInteger(Destination) << 11;
	Value |= static_cast<uint64_t>(id) << 56;
	if (Flags & ACTIVE_HIGH_LOW) Value |= (1 << 13);
	if (Flags & EDGE_LEVEL) Value |= (1 << 15);
	if (Flags & MASKED) Value |= (1 << 16);
	this->WriteEntry(i, Value);
}

IOAPIC *IOAPICForGSI(uint32_t GSI) {
	IOAPIC *Result = nullptr;
	IOAPICs.ForEach(
	    [&](IOAPIC &IOAPIC, int Index) {
		    auto [Start, End] = IOAPIC.GSIRange();
		    if (Start <= GSI && End >= GSI) Result = &IOAPIC;
	    });
	return Result;
}

void Initialize()
{
	Log("Starting IOAPIC...");
	Initialized = true;
	Log("Done!");
}
} // namespace IOAPIC
} // namespace Amd64
} // namespace Neon
