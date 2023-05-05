#pragma once

#include <Core/Process.h>

namespace Neon {
namespace Scheduler {
void Initialize();
void AddThread(Thread *Thread);
void RemoveThread(Thread *Thread);
void Yield();
void Schedule();
void SwitchThreads(Thread *NewThread);
void ScheduleProcess(Process *Process);
void UnscheduleProcess(Process *Process);
} // namespace Scheduler
} // namespace Neon
