#include <Common/Log.h>
#include <Common/Queue.h>
#include <Common/Vector.h>
#include <Core/Process.h>
#include <Core/Scheduler.h>

namespace Neon {
namespace Scheduler {
Queue<Thread *> *ReadyQueue[31];
Vector<Process *> Processes;
Thread *CurrentThread = ThisThread();
lock_t Lock;

uint64_t GetTicks() {
	static const uint64_t TicksPerSecond = 1000;
	static uint64_t Result = 0;
	RTC::Time time = RTC::GetTime();
	uint64_t Seconds = Time.Seconds + 60 * Time.Minutes + 3600 * Time.Hours;
	uint64_t Milliseconds = 1000 * Seconds + Time.Milliseconds;
	Result = Milliseconds * TicksPerSecond / 1000;
	return Result;
}

void Initialize() {
	for(auto &i : ReadyQueue) i = new Queue<Thread *>();
	Processes.ForEach([&](Process &Process, int Index){
		Process.Threads.ForEach([&](Thread &Thread, int Index) {
			Lock.Lock();
			int Priority = Thread.Priority;
			ReadyQueue[Priority]->Push(Thread);
			Lock.Unlock();
			Thread.Enqueued = true;
		});
	});
}

void AddThread(Thread *Thread) {
	Lock.Lock();
	int Priority = Thread->Priority - 1;
	ReadyQueue[Priority]->Push(Thread);
	Lock.Unlock();
}

void RemoveThread(Thread *Thread) {
	Lock.Lock();
	int Priority = Thread->Priority - 1;
	ReadyQueue[Priority]->Remove(Thread);
	Lock.Unlock();
}

void Yield() {
	Lock.Lock();
	CurrentThread->CurrentState = Thread::Waiting;
	CurrentThread->LastSwitchTime = GetTicks();
	int Priority = CurrentThread->Priority - 1;
	if (CurrentThread->Quantum <= 0) {
		CurrentThread->Quantum = Quantum;
		ReadyQueue[Priority]->Push(CurrentThread);
	}
	Thread *NextThread = nullptr;
	for(auto &i : ReadyQueue) {
		if(!i->Empty()) {
			NextThread = i->Front();
			i->Pop();
			break;
		}
	}
	Lock.Unlock();
	if(!NextThread) return;
	NextThread->CurrentState = Thread::Running;
	NextThread->TotalRunTime += Scheduler::GetTicks() - NextThread->LastSwitchTime;
	NextThread->LastSwitchTime = Scheduler::GetTicks();
	NextThread->Quantum = CurrentThread->Quantum - (Scheduler::GetTicks() - CurrentThread->LastSwitchTime);
	SwitchThreads(NextThread);
}

void Schedule() {
	Thread *NextThread = nullptr;
	for(auto &i : ReadyQueue) {
		if(!i->Empty()) {
			NextThread = i->Front();
			i->Pop();
			break;
		}
	}
	if(!NextThread) return;
	Lock.Lock();
	NextThread->CurrentState = Thread::Running;
	SwitchThreads(NextThread);
	Lock.Unlock();
}

void SwitchThreads(Thread *NewThread) { CurrentThread = NewThread; }

void ScheduleProcess(Process *Process) {
	for(auto &Thread : Process->Threads) {
		if(Thread->Enqueued) continue;
		AddThread(Thread);
		Thread->Enqueued = true;
	}
}

void UnscheduleProcess(Process *Process) {
	for(auto &Thread : Process->Threads) {
		if(!Thread->Enqueued) continue;
		RemoveThread(Thread);
		Thread->Enqueued = false;
	}
}
} // namespace Scheduler
} // namespace Neon
