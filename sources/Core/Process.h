#pragma once

#include <Amd64/Amd64.h>
#include <Common/String.h>
#include <Common/Vector.h>
#include <Common/Map.h>
#include <type_traits>

namespace Neon {
class Thread;
class Process {
	friend class Thread;
public:
	String Name;
	int Priority, Id;
	Vector<Thread *> Threads;
	Vector<Process *> Children;
	Amd64::VirtualMemoryManager::Pagemap Pagemap;
	void operator=(Process &Other) {
		std::swap(this->Name, Other->Name);
		std::swap(this->Children, Other->Children);
		std::swap(this->Threads, Other->Threads);
	}
	int AllocatePID();
	Process(String Name, VirtualMemoryManager::Pagemap Pagemap)
	    : Name(Name), Pagemap(Pagemap), Priority(1) {}
	void Sleep(int32_t Seconds);
	void Sleep(uint32_t Milliseconds);
	void Sleep(uint64_t Nanoseconds);
};

class Thread {
	using namespace Amd64;
	friend class Process;
	Amd64::registers_t AllocatedRegisters;
	uint64_t FPUStorageSize = 512;
	void (*FPUSave)(uint8_t *);
	void (*FPURestore)(uint8_t *);
public:
	Process &Parent;
	bool Enqueued;
	enum State { Running, Waiting, Dying, Blocked, Terminated };
	State CurrentState;
	int Priority;
	uint64_t Quantum, TotalRunTime, LastSwitchTime, AffinityMask;
	Thread(uintptr_t sp, uintptr_t ip, Process &Parent)
	    : AllocatedRegisters{0}, CurrentState{Waiting}, Quantum{0}
	    , TotalRunTime{0}, LastSwitchTime{0}, Priority{1}, AffinityMask{0}
	    , Parent{Parent}, Enqueued{false} {
		this->AllocatedRegisters.rsp = sp;
		this->AllocatedRegisters.rip = ip;
	}
	Thread() = default;
	void Load(registers_t *Registers) { *Registers = this->AllocatedRegisters; }
	void Save(const registers_t *Registers) {
		this->AllocatedRegisters = *Registers;
	}
	int AllocateTID();
	void Sleep(int32_t Seconds);
	void Sleep(uint32_t Milliseconds);
	void Sleep(uint64_t Nanoseconds);
	void Yield();
};

struct ProcessMetadata {
	int ID;
	String Name;
};

Map<ProcessMetaData, Process &> Processes;
Map<int, Thread &> Threads;

extern "C" Thread *ThisThread();
extern "C" Process *ThisProcess();
Process CreateProcess(StringView &Name, Amd64::Pagemap Pagemap);
} // namespace Neon
