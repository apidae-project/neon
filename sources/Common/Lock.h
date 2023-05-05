#pragma once

#include <atomic>

// https://rigtorp.se/spinlock/
namespace Neon {
struct lock_t {
	std::atomic<bool> Lock_ = {0};
	void Lock() noexcept;
	bool TryLock() noexcept;
	void Unlock() noexcept;
};
} // namespace Neon