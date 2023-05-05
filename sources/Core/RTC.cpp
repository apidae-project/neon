#include <Core/RTC.h>
#include <Amd64/Amd64.h>

namespace Neon {
namespace RTC {
uint8_t ReadRegister(Registers Register) {
	Amd64::outb(CMOSAddress, static_cast<uint8_t>(Register));
	return Amd64::inb(CMOSData);
}

uint8_t BCDToBinary(uint8_t BCD) {
	return (BCD & 0x0F) + ((BCD >> 4) * 10);
}

Time GetTime() {
	Time Result;
	uint8_t Seconds = ReadRegister(Registers::Seconds);
	Result.Seconds = BCDToBinary(Seconds);
	uint8_t Minutes = ReadRegister(Registers::Minutes);
	Result.Minutes = BCDToBinary(Minutes);
	uint8_t Hours = ReadRegister(Registers::Hours);
	if(Hours & 0x80) {
		Result.Hours = BCDToBinary(Hours & 0x7F);
	} else {
		bool PM = Hours & 0x20;
		Result.Hours = BCDToBinary(Hours & 0x1F);
		if (PM && Result.Hours < 12) Result.Hours += 12;
	}
	uint8_t DayOfMonth = ReadRegister(Registers::DayOfMonth);
	Result.DayOfMonth = BCDToBinary(DayOfMonth);
	uint8_t Month = ReadRegister(Registers::Month);
	Result.Month = BCDToBinary(Month);
	uint8_t Year = ReadRegister(Registers::Year);
	Result.Year = BCDToBinary(Year) + 2000;
	uint8_t Status = ReadRegister(Registers::StatusB);
	uint8_t Century = 0;
	if(Status & 0x02) Century = 100;
	uint8_t LowByte = ReadRegister(Registers::Seconds);
	uint8_t HighByte = ReadRegister(Registers::StatusB);
	int Count = (HighByte & 0x80) ? (LowByte | 0xFF00) : LowByte;
	Result.Milliseconds = (Count * 10) / 32768;
	ReadRegister(Registers::DayOfWeek);
	ReadRegister(Registers::StatusB);
	return Result;
}

void Initialize() {
	Amd64::outb(CMOSAddress, 0x80);
	uint8_t StatusB = ReadRegister(Registers::StatusB);
	Amd64::outb(CMOSAddress, static_cast<uint8_t>(Registers::StatusB));
	Amd64::outb(CMOSData, StatusB | 0x40);
	Amd64::outb(CMOSAddress, static_cast<uint8_t>(Registers::StatusB));
	Amd64::outb(CMOSData, StatusB & 0xFE);
}
}
}
