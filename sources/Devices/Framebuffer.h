#pragma once

#include <cstddef>
#include <limine.h>

namespace Neon
{
namespace Devices
{
namespace Framebuffer
{
extern limine_framebuffer **Framebuffers;
extern limine_framebuffer *MainFramebuffer;
extern uint64_t FramebufferCount;
void Initialize();
} // namespace Framebuffer
} // namespace Devices
} // namespace Neon
