#include <Common/Log.h>
#include <Devices/Framebuffer.h>
#include <LimineRequests.h>
#include <limine.h>

namespace Neon
{
namespace Devices
{
namespace Framebuffer
{
limine_framebuffer **Framebuffers;
limine_framebuffer *MainFramebuffer;
uint64_t FramebufferCount;

void Initialize()
{
	Log("[Framebuffer] Initializing...");
	Framebuffers = FrameBufferRequest.response->framebuffers;
	FramebufferCount = FrameBufferRequest.response->framebuffer_count;
	MainFramebuffer = Framebuffers[0];
	Log("[Framebuffer] Done!");
}
} // namespace Framebuffer
} // namespace Devices
} // namespace Neon
