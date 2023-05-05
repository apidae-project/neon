#pragma once

#include <cstdint>

namespace Neon
{
#define TEST_MODE 1
struct Processor
{
	size_t id = 0;
	uint64_t fpu_storage_size = 512;
	void (*fpu_save)(uint8_t *);
	void (*fpu_restore)(uint8_t *);
	volatile bool Up = false;
};
} // namespace Neon
