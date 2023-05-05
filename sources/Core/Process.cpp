#include <Core/Process.h>
#include <Core/RTC.h>
#include <Core/Scheduler.h>
#include <Amd64/Amd64.h>
#include <Common/Lock.h>
#include <Common/StringView.h>

namespace Neon {
int Process::AllocatePID() {
	lock_t Lock;
	Lock.Lock();
	static int NextPID = 1;
	NextPID++;
	Lock.Unlock();
	return NextPID;
}

int Thread::AllocateTID() {
	lock_t Lock;
	Lock.Lock();
	static int NextTID = 1;
	NextTID++;
	Lock.Unlock();
	return NextTID;
}

void Thread::Sleep(int32_t Seconds) {
	uint64_t TargetTime = RTC::GetTime().ToNanoseconds() + Seconds * 1000000000UL;
	while(RTC::GetTime().ToNanoseconds() < TargetTime) {}
}

void Thread::Sleep(uint32_t Milliseconds) {
	uint64_t TargetTime = RTC::GetTime().ToNanoseconds() + Milliseconds * 1000000UL;
	while(RTC::GetTime().ToNanoseconds() < TargetTime) {}
}

void Thread::Sleep(uint64_t Nanoseconds) {
	uint64_t TargetTime = RTC::GetTime().ToNanoseconds() + Nanoseconds;
	while(RTC::GetTime().ToNanoseconds() < TargetTime) {}
}

void Thread::Yield() {
	Scheduler::Yield();
}

void Process::Sleep(int32_t Seconds) {
	uint64_t TargetTime = RTC::GetTime().ToNanoseconds() + Seconds * 1000000000UL;
	while(RTC::GetTime().ToNanoseconds() < TargetTime) {}
}

void Process::Sleep(uint32_t Milliseconds) {
	uint64_t TargetTime = RTC::GetTime().ToNanoseconds() + Milliseconds * 1000000UL;
	while(RTC::GetTime().ToNanoseconds() < TargetTime) {}
}

void Process::Sleep(uint64_t Nanoseconds) {
	uint64_t TargetTime = RTC::GetTime().ToNanoseconds() + Nanoseconds;
	while(RTC::GetTime().ToNanoseconds() < TargetTime) {}
}

Process CreateProcess(StringView &Name
                      , Amd64::VirtualMemoryManager::Pagemap Pagemap) {
	Process Result = Process(Name, Pagemap);
	Processes.Push({{Result.AllocatePID(), Result.Name}, Result});
	Result.Threads.ForEach([&](Thread &Thread, int Index) {
		Threads.Push({Thread.AllocateTID(), Thread});
	});
	return Result;
}

Thread CreateThread(Process &Parent, uintptr_t ip, uintptr_t sp) {
	Thread Result = Thread(ip, sp, Parent);
	Threads.Push(Result.AllocateTID(), Result);
	return Result;
}
}