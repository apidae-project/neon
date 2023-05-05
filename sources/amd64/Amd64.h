#pragma once

#include <Amd64/Cpuid.h>
#include <Common/CString.h>
#include <Function.h>
#include <Common/Lock.h>
#include <LimineRequests.h>
#include <Neon.h>
#include <Pair.h>
#include <cstddef>
#include <cstdint>
#include <Utility.h>

namespace Neon {
namespace Amd64 {
static constexpr uint64_t CPUID_INVARIANT_TSC = (1 << 8);
static constexpr uint64_t CPUID_TSC_DEADLINE = (1 << 24);
static constexpr uint64_t CPUID_SMEP = (1 << 7);
static constexpr uint64_t CPUID_SMAP = (1 << 20);
static constexpr uint64_t CPUID_UMIP = (1 << 2);
static constexpr uint64_t CPUID_X2APIC = (1 << 21);
static constexpr uint64_t CPUID_GBPAGE = (1 << 26);

enum PAT {
	Uncachable = 0x00,
	WriteCombining = 0x01,
	WriteThrough = 0x04,
	WriteProtected = 0x05,
	WriteBack = 0x06,
	Uncached = 0x07
};

typedef struct [[gnu::packed]] {
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
	uint64_t int_no, error_code, rip, cs, rflags, rsp, ss;
} registers_t;

uint64_t rdmsr(uint32_t msr);
void wrmsr(uint32_t msr, uint64_t value);

void SetKernelGS(uint64_t Address);
uint64_t GetKernelGS();

void SetGS(uint64_t Address);
uint64_t GetGS();

void SetFS(uint64_t Address);
uint64_t GetFS();

void WriteCR(uint64_t reg, uint64_t val);
uint64_t ReadCR(uint64_t reg);

void wrxcr(uint32_t i, uint64_t value);

void xsaveopt(uint8_t *region);
void xsave(uint8_t *region);
void xrstor(uint8_t *region);
void fxsave(uint8_t *region);
void fxrstor(uint8_t *region);

void invlpg(uint64_t Address);

void EnableSSE();
void EnableSMEP();
void EnableSMAP();
void EnableUMIP();
void EnablePAT();
void outb(uint16_t Port, uint8_t Value);
void outw(uint16_t Port, uint16_t Value);
void outl(uint16_t Port, uint32_t Value);
uint8_t inb(uint16_t Port);
uint16_t inw(uint16_t Port);
uint32_t inl(uint16_t Port);
void Wait(void);
void EnableCPUFeatures();

#define ReadGS(Offset)                                                       \
	({                                                                       \
		uint64_t Value;                                                      \
		asm volatile("movq %%gs:[" #Offset "], %0" : "=r"(Value)::"memory"); \
		Value;                                                               \
	})

#define ReadCR(Register)                                                    \
	({                                                                      \
		uint64_t Value;                                                     \
		asm volatile("movq %%cr" #Register ", %0" : "=r"(Value)::"memory"); \
		Value;                                                              \
	})

#define WriteCR(Register, Value)                                         \
	{                                                                    \
		asm volatile("movq %0, %%cr" #Register ::"r"(Value) : "memory"); \
	}

#define rdtsc()                                \
	({                                         \
		uint64_t __t;                          \
		__asm__ volatile("rdtsc" : "=A"(__t)); \
		__t;                                   \
	})

namespace GDT {
static constexpr uint8_t GDT_NULL = 0x00;
static constexpr uint8_t GDT_CODE = 0x08;
static constexpr uint8_t GDT_DATA = 0x10;
static constexpr uint8_t GDT_USER_CODE = 0x18;
static constexpr uint8_t GDT_USER_DATA = 0x20;
static constexpr uint8_t GDT_TSS = 0x28;

struct [[gnu::packed]] GDTR {
	uint16_t Size;
	uint64_t Offset;
};

struct [[gnu::packed]] GDTEntry {
	uint16_t Limit0;
	uint16_t Base0;
	uint8_t Base1;
	uint8_t Access;
	uint8_t Granularity;
	uint8_t Base2;
};

struct [[gnu::packed]] TSSEntry {
	uint16_t Length;
	uint16_t Base0;
	uint8_t Base1;
	uint8_t Flags1;
	uint8_t Flags2;
	uint8_t Base2;
	uint32_t Base3;
	uint32_t Reserved;
};

struct [[gnu::packed]] GDT {
	GDTEntry Null;
	GDTEntry Code;
	GDTEntry Data;
	GDTEntry UserCode;
	GDTEntry UserData;
	TSSEntry Tss;
};

struct [[gnu::packed]] TSS {
	uint32_t Reserved0;
	uint64_t RSP[3];
	uint64_t Reserved1;
	uint64_t IST[7];
	uint64_t Reserved2;
	uint16_t Reserved3;
	uint16_t IOPBOffset;
};

extern TSS *tss;

void Initialize();
} // namespace GDT

namespace IDT {
using namespace GDT;
struct InterruptFrame {
	uint64_t error_code;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
};

struct [[gnu::packed]] IDTDescriptor {
	uint16_t Limit;
	uint64_t Offset;
};

struct [[gnu::packed]] IDTGate {
	uint16_t Offset1;
	uint16_t Selector;
	uint8_t IST;
	uint8_t Type;
	uint16_t Offset2;
	uint32_t Offset3;
	uint32_t Zero;
};

struct [[gnu::packed]] IDTEntry {
	uint16_t Offset1;
	uint16_t Selector;
	uint8_t IST;
	uint8_t TypeAttr;
	uint16_t Offset2;
	uint32_t Offset3;
	uint32_t Zero;
	void Set(void *ISR, uint8_t typeattr = 0x8E, uint8_t ist = 0) {
		uint64_t ISRAddress = reinterpret_cast<uint64_t>(ISR);
		this->Offset1 = static_cast<uint16_t>(ISRAddress);
		this->Selector = GDT_CODE;
		this->IST = ist;
		this->TypeAttr = typeattr;
		this->Offset2 = static_cast<uint16_t>(ISRAddress >> 16);
		this->Offset3 = static_cast<uint32_t>(ISRAddress >> 32);
		this->Zero = 0;
	}
};

struct [[gnu::packed]] IDTPointer {
	uint16_t Limit;
	uint64_t Base;
};

class InterruptHandler {
	Cxxutil::Function<void(registers_t *)> Handler;
public:
	template <typename Function, typename... Args>
	bool Set(Cxxutil::Function &&func, Args... args) {
		if (this->Handler == true) return false;
		this->Handler = [func = Forward<Cxxutil::Function>(func),
		                 args...](registers_t *Registers) mutable
		{ func(Registers, Move(args)...); };
		return true;
	}

	bool Clear();
	bool Get();
	bool operator()(registers_t *Registers);
};

void Initialize();
} // namespace IDT

namespace IOAPIC {
enum class DeliveryMode : uint8_t {
	FIXED = 0b000,
	LOW_PRIORITY = 0b001,
	SMI = 0b010,
	NMI = 0b100,
	INIT = 0b101,
	EXT_INT = 0b111
};

enum class DestinationMode : uint8_t {
	PHYSICAL = 0,
	LOGICAL = 1
};

enum Flags {
	MASKED = (1 << 0),
	ACTIVE_HIGH_LOW = (1 << 1),
	EDGE_LEVEL = (1 << 3),
};

class IOAPIC {
public:
	bool Initialized;
	uint32_t Read(uint32_t Register);
	void Write(uint32_t Register, uint32_t Value);
	uint32_t ReadEntry(uint32_t Entry);
	void WriteEntry(uint32_t Entry, uint64_t Value);
	void Mask(uint8_t InterruptRequest);
	void Unmask(uint8_t InterruptRequest);
	void Set(uint8_t i,
	         uint8_t Vector,
	         DeliveryMode Delivery,
	         DestinationMode Destination,
	         uint16_t Flags,
	         uint8_t id);
	Cxxutil::Pair<uint32_t, uint32_t> GSIRange() const {
		return {this->GSIBase, this->GSIBase + Redirections};
	}
	IOAPIC();
private:
	constexpr uint32_t Entry(uint32_t i) { return 0x10 + (i * 2); }
	uintptr_t MMIOBase = 0;
	uint32_t GSIBase = 0;
	size_t Redirections = 0;
};

void Initialize();
} // namespace IOAPIC

template <std::unsigned_integral Type> static inline Type mmin(auto Address) {
	volatile auto Pointer = reinterpret_cast<volatile Type *>(Address);
	return *Pointer;
}

template <std::unsigned_integral Type>
static inline void mmout(auto Address, Type Value) {
	volatile auto Pointer = reinterpret_cast<volatile Type *>(Address);
	*Pointer = Value;
}

namespace VirtualMemoryManager {
enum Flags {
	Present = (1 << 0),
	Write = (1 << 1),
	UserSuper = (1 << 2),
	WriteThrough = (1 << 3),
	CacheDisable = (1 << 4),
	Accessed = (1 << 4),
	LargerPages = (1 << 5),
	PAT = (1 << 7),
	Custom0 = (1 << 9),
	Custom1 = (1 << 10),
	Custom2 = (1 << 11),
	NoExec = (1UL << 63)
};

struct PDEntry {
	uint64_t Value = 0;

	void SetFlags(uint64_t Flags, bool Enabled) {
		uint64_t BitSel = Flags;
		this->Value &= ~BitSel;
		if (Enabled) this->Value |= BitSel;
	}

	bool GetFlags(uint64_t flags) {
		return (this->Value & flags) ? true : false;
	}

	uint64_t GetAddress() { return (this->Value & 0x000FFFFFFFFFF000) >> 12; }

	void SetAddress(uint64_t Address) {
		Address &= 0x000000FFFFFFFFFF;
		this->Value &= 0xFFF0000000000FFF;
		this->Value |= (Address << 12);
	}
};

struct [[gnu::aligned(0x1000)]] PageTable { PDEntry Entries[512]; };

struct Pagemap {
	PageTable *TopLevel = nullptr;
	lock_t Lock;
	uint64_t LargePageSize = 0x200000;
	uint64_t PageSize = 0x1000;

	PDEntry *VirtualToPageTableEntry(uint64_t VirtualAddress,
	                                 bool Allocate = true,
	                                 bool HugePages = false) const;

	bool Map(uint64_t VirtualAddress,
	         uint64_t PhysicalAddress,
	         uint64_t Flags = (Flags::Present | Flags::Write),
	         bool HugePages = false);

	bool Remap(uint64_t OldVirtualAddress,
	           uint64_t NewVirtualAddress,
	           uint64_t Flags = (Flags::Present | Flags::Write),
	           bool HugePages = false);

	bool Unmap(uint64_t VirtualAddress, bool HugePages = false);

	uint64_t VirtualToPhysical(uint64_t VirtualAddress, bool HugePages = false) {
		PDEntry *PMLEntry =
		    this->VirtualToPageTableEntry(VirtualAddress, false, HugePages);
		if (PMLEntry == nullptr || !PMLEntry->GetFlags(Present)) return 0;
		return PMLEntry->GetAddress() << 12;
	}

	void SwitchTo() const;
	void Save();
	Pagemap(bool User = false);
};
void Initialize();
} // namespace VirtualMemoryManager

Processor Processors;
extern "C" Processor *ThisCPU();

namespace HPET {
class GenericHPET {
	constexpr uint16_t IdRegisterOffset = 0x00;
	constexpr uint16_t ConfigRegisterOffset = 0x10;
	constexpr uint16_t CounterRegisterOffset = 0xF0;
	constexpr uint64_t IdRevisionMask = 0xFFFFFFFF00000000;
	constexpr uint64_t IdClockPeriodMask = 0x00000000FFFFFFFF;
	constexpr uint64_t ConfigEnableMask = 0x01;
	constexpr uint64_t ConfigLegacyReplacementMask = 0x02;
	constexpr uint64_t CounterValueMask = 0xFFFFFFFFFFFFFFFF;

	const uint64_t ClockPeriod = Registers_.GeneralCapabilities
	                             & IdClockPeriodMask;

	struct TimerRegisters {
		uint64_t Configuration;
		uint64_t Reserved;
		uint64_t ComparatorValue;
	};

	class Timer {
		TimerRegisters &Registers_;
	public:
		Timer(TimerRegisters &Registers) : registers_(Registers) {}
		uint64_t GetComparatorValue() const { return Registers_.VomparatorValue; }
		void SetComparatorValue(uint64_t Value) {
			Registers_.ComparatorValue = Value;
		}
	};

	struct Registers {
		uint64_t GeneralCapabilities;
		uint64_t Reserved1;
		uint64_t GeneralConfiguration;
		uint64_t Reserved2;
		uint64_t InterruptStatus;
		uint64_t Reserved3;
		uint64_t MainCounterValue;
		uint64_t reserved4[24];
		TimerRegisters Timers[32];
	};

	Registers &Registers_;
public:
	explicit HPET(uintptr_t BaseAddress)
	    : registers_(*reinterpret_cast<Registers *>(BaseAddress)) {}

	void Enable() { Registers_.GeneralConfiguration |= ConfigEnableMask; }
	void Disable() { Registers_.GeneralConfiguration &= ~ConfigEnableMask; }

	uint64_t GetMainCounterValue() const { return Registers_.MainCounterValue; }

	Timer GetTimer(const int Index) { return Timer(Registers_.Timers[Index]); }

	void Wait(const int Index = 0, const uint64_t Ticks) {
		const uint64_t StartValue = GetTimerValue(Index);
		while (GetTimerValue(Index) - StartValue < Ticks) {}
	}
	void Delay(const int Index = 0, const uint64_t Microseconds) {
		uint64_t CounterValue = GetMainCounterValue();
		uint64_t Delay = Microseconds * ClockPeriod;
		while(GetMainCounterValue() - CounterValue < DelayValue)
	}

	void Initialize();
};

void Initialize();
} // namespace HPET
} // namespace Amd64
} // namespace Neon
