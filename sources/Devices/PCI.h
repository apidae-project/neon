#pragma once

#include <cstddef>
#include <concepts>

namespace Neon {
namespace Devices {
namespace PCI {
struct Device {
	int64_t Parent;
	bool MultiFunction, HasPRT;
	uint16_t DeviceID, VendorID, RevisionID;
	uint8_t Function, Device, Bus, IRQPin;
	uint8_t DeviceClass, Device_;
	uint32_t GSI;
	void GetAddress(uint32_t Offset);
	template<typename T>
	requires std::is_integral_v<T> && UnsignedType<T> T Read(uint32_t Offset);
	template<UnsignedType Type> void Write(uint32_t Offset, Type Value);
};

struct Bar {
	uintptr_t Base;
	size_t Size;
	bool IsMMIO, IsPrefetchable;
};

#define MSI_OPT 2
#define MSI_ADDR_LOW 0x4
#define MSI_DATA_32 0x8
#define MSI_DATA_64 0xC
#define MSI_64BIT_SUPPORTED (1 << 7)

union[gnu::packed] MSIAddress {
	struct {
		uint32_t Reserved0 : 2;
		uint32_t DestinationMode : 1;
		uint32_t RedirectionHint : 1;
		uint32_t Reserved1 : 8;
		uint32_t DestinationId : 8;
		uint32_t BaseAddress : 12;
	};
	uint32_t Raw;
};

Device FindDevice(uint8_t Class, uint8_t Function, uint16_t Bus);
void Initialize();
} // namespace PCI
} // namespace Devices
} // namespace Neon
