#include <Common/Lock.h>
#include <atomic>

namespace Neon {
void lock_t::Lock() noexcept {
	for(;;) {
		if(!Lock_.exchange(true, std::memory_order_acquire)) return;
		while(Lock_.load(std::memory_order_relaxed)) asm("pause");
	}
}

bool lock_t::TryLock() noexcept {
	return !Lock_.load(std::memory_order_relaxed) &&
	       !Lock_.exchange(true, std::memory_order_acquire);
}

void lock_t::Unlock() noexcept {
	Lock_.store(false, std::memory_order_release);
}
} // namespace Neon