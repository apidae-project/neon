#pragma once

#include <cstddef>

namespace Neon {
namespace RTC {
constexpr uint16_t CMOSAddress = 0x70;
constexpr uint16_t CMOSData = 0x71;
struct Time {
	int Milliseconds, Seconds, Minutes, Hours, DayOfMonth, Month,
	    Year, WeekDay, YearDay;
	bool IsDST;

	uint64_t ToNanoSeconds() const {
		return ((static_cast<uint64_t>(Hours) * 60 + Minutes) * 60 + Seconds) * 1000 + Milliseconds * 1000000;
	}

	uint64_t ToMicroSeconds() const {
		return ((static_cast<uint64_t>(Hours) * 60 + Minutes) * 60 + Seconds) * 1000000 + Milliseconds * 1000;
	}

	uint64_t ToMilliSeconds() const {
		return ((static_cast<uint64_t>(Hours) * 60 + Minutes) * 60 + Seconds) * 1000 + Milliseconds;
	}
};

enum class Registers {
	Seconds = 0,
	Minutes = 2,
	Hours = 4,
	DayOfWeek = 6,
	DayOfMonth = 7,
	Month = 8,
	Year = 9,
	StatusA = 10,
	StatusB = 11,
};

uint8_t ReadRegister(Registers Register);
uint8_t BCDToBinary(uint8_t BCD);
Time GetTime();
void Initialize();
}
}
