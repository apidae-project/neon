#include <Core/Mutex.h>
#include <Core/Process.h>

namespace Neon {
void Mutex::Acquire() {
	uint32_t Retries = 0;
	while(true) {
		if(Flag.test_and_set(std::memory_order_acquire)) {
			if(Retries++ >= 4) {
				ThisProcess().Sleep(0);
				Retries = 0;
			}
		} else {
			Owner = &ThisThread();
			return;
		}
	}
}


void Mutex::Release() {
	if(Owner != &ThisThread()) return;
	Owner = nullptr;
	Flag.clear(std::memory_order_release);
}
} // namespace Neon