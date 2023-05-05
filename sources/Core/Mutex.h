#include <Core/Mutex.h>
#include <Core/Process.h>

namespace Neon {
void Mutex::Acquire() {
	uint32_t Spins = 0;
	uint32_t Backoff = 1;
	while(true) {
		if(!Flag.test_and_set(std::memory_order_acquire)) {
			Owner = &ThisThread();
			return;
		}

		if(Spins++ > 16) {
			ThisProcess().Sleep(Backoff);
			Backoff *= 2;
			Spins = 0;
		}
	}
}

void Mutex::Release() {
	if (Owner != &ThisThread()) return;
	Owner = nullptr;
	Flag.clear(std::memory_order_release);
}
} // namespace Neon
