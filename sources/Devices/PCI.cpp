#include <Devices/PCI.h>
#include <Vector.h>
#include <Amd64/Amd64.h>
#include <concepts>
#include <cstdint>
#include <algorithm>

namespace Neon {
namespace Devices {
namespace PCI {
Vector<Device> Devices;

template<typename Type>
concept UnsignedType = (std::same_as<Type, uint8_t> ||
                        std::same_as<Type, uint16_t> ||
                        std::same_as<Type, uint32_t>);

void Device::GetAddress(uint32_t Offset) {
	uint32_t Address = (this->Bus << 16) | (this->Device_ << 11) |
	                   (this->Function << 8) | (Offset & ~(uint32_t(3))) |
	                   0x80000000;
	Amd64::outl(0xcf8, Address);
}

template<typename T>
requires std::is_integral_v<T> && UnsignedType<T> Device::Read(uint32_t Offset) {
	this->GetAddress(Offset);
	T Value = 0;
	switch(Offset & 3) {
		case 0:
			Value = Amd64::inl(0xcfc);
			break;
		case 1:
			Value = Amd64::inw(0xcfc + (Offset & 2));
			break;
		case 2:
			Value = Amd64::inb(0xcfc + (Offset & 3));
			break;
		default:
			break;
	}
	return Value;
}

template<UnsignedType Type> void Device::Write(Type Offset, Type Value) {
	this->GetAddress(Offset);
	switch(Offset & 3) {
		case 0:
			Amd64::outl(0xcfc, Value);
			break;
		case 1:
			Amd64::outw(0xcfc, Value);
			break;
		case 2:
			Amd64::outb(0xcfc, Value);
			break;
		default:
			break;
	}
}

Vector<Device> DiscoverDevices() {
	Vector<Device> Result;
	for(uint8_t Bus = 0; Bus < 256; Bus++) {
		for(uint8_t Device_ = 0; Device_ < 32; Device_++) {
			for(uint8_t Function = 0; Function < 8; Function++) {
				Device Temp;
				Temp.Bus = Bus;
				Temp.Device_ = Device_;
				Temp.Function = Function;
				Temp.Device = (Temp.Device_ >> 3) & 0x1f;
				uint16_t VendorID = Temp.Read<uint16_t>(0);
				if(VendorID == 0xFFFF) continue;
				uint16_t DeviceID = Temp.Read<uint16_t>(2);
				Temp.DeviceID = DeviceID;
				Temp.VendorID = VendorID;
				Temp.RevisionID = Temp.Read<uint8_t>(8);
				Temp.DeviceClass = Temp.Read<uint8_t>(0xB);
				uint32_t HeaderType = Temp.Read<uint8_t>(0xE & 0x7F);
				Temp.MultiFunction = (HeaderType == 0);
				Temp.IRQPin = Temp.Read<uint8_t>(0x3D);
				Result.PushBack(Temp);
				if(Temp.MultiFunction) {
					for(uint8_t Func = 1; Func < 8; Func++) {
						Device MultiFunctionTemp;
						MultiFunctionTemp = Temp;
						MultiFunctionTemp.Function = Func;
						uint16_t MultifunctionVendorID =
						    MultiFunctionTemp.Read<uint16_t>(0);
						if(MultifunctionVendorID != 0xFFFF) {
							MultiFunctionTemp.DeviceID =
							    MultiFunctionTemp.Read<uint16_t>(2);
							MultiFunctionTemp.VendorID = MultifunctionVendorID;
							MultiFunctionTemp.RevisionID =
							    MultiFunctionTemp.Read<uint8_t>(8);
							MultiFunctionTemp.DeviceClass =
							    MultiFunctionTemp.Read<uint8_t>(0xB);
							MultiFunctionTemp.IRQPin =
							    MultiFunctionTemp.Read<uint8_t>(0x3D);
							Result.PushBack(MultiFunctionTemp);
						}
					}
				}
			}
		}
	}
	return Result;
}

Vector<uint32_t> AllocateMSIVectors(Device &Device_, size_t Count) {
	Vector<uint32_t> Result;
	if (!(Device_.Capabilities & Capabilities::MSI)) return Result;
	uint32_t Cap = Device_.Read<uint32_t>(Capabilities::MSI);
	uint32_t CountMask = (Cap >> 16) & 0xff;
	if(CountMask == 0) CountMask = 1;
	else CountMask = 1 << CountMask;
	Count = std::min(Count, CountMask);
	Result.Reserve(Count);
	for(size_t i = 0; i < Count; i++) {
		Device_.Write<uint16_t>(Capabilities::MSI, 0x4000 | (i << 4));
		uint32_t addr_low = Device_.Read<uint32_t>(MSI_ADDR_LOW);
		if(Cap & MSI_64BIT_SUPPORTED) {
			uint32_t addr_high = Device_.Read<uint32_t>(MSI_ADDR_LOW + 4);
			MSIAddress Address = {};
			Address.BaseAddress = addr_low & ~0xfff;
			Address.Raw = (addr.Raw & 0xffff0000) | (addr_high & 0xffff);
			Device_.Write<uint32_t>(MSI_ADDR_LOW, Address.Raw);
		}
		else Device_.Write<uint32_t>(MSI_ADDR_LOW, addr_low & ~0xfff);
		uint32_t Data = (1 << 31) | i;
		if(Cap & MSI_64BIT_SUPPORTED) {
			Device_.Write<uint32_t>(MSI_DATA_64, 0);
			Device_.Write<uint32_t>(MSI_DATA_64 + 4, Data);
		}
		else Device_.Write<uint32_t>(MSI_DATA_32, Data);
		Result.Push(addr_low & ~0xfff);
	}
	return Result;
}

void Initialize() {
	Log("[PCI] Initializing...");
	Log("[PCI] Found {} devices.", Devices.Size());
	Devices = DiscoverDevices();
	Devices.ForEach([](Device &Element, int Index) {
		uint16_t CommandRegister = Element.Read<uint16_t>(0x04);
		if(CommandRegister & (1 << 2)) {
			CommandRegister |= (1 << 10);
			Element.Write<uint16_t>(0x04, CommandRegister);
		}
		if(Element.Read<uint16_t>(0x0A) & (1 << 3)) {
			size_t Vectors = 1;
			uint32_t GSI = Element.GSI;
			if(Element.Read<uint16_t>(0x10) & MSI_64BIT_SUPPORTED) Vectors = 2;
			uint32_t MSIAddress = AllocateMSIVectors(Element, Count);
			Element.Write<uint32_t>(MSI_OPT, 0x00010000 | (Vectors - 1));
			Element.Write<uint32_t>(MSI_ADDR_LOW, MSIAddress);
			Element.Write<uint32_t>(MSI_OPT, GSI);
			if(Vectors == 2) Element.Write<uint32_t>(MSI_DATA_64, 0);
		}
	});
	Log("[PCI] Done!");
}
} // namespace PCI
} // namespace Devices
} // namespace Neon
