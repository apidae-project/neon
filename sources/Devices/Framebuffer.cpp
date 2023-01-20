#include <LimineRequests.h>
#include <Devices/Framebuffer.h>
#include <limine.h>

namespace Neon {
    namespace Devices {
        limine_framebuffer **Framebuffers;
        limine_framebuffer *MainFramebuffer;
        uint64_t FramebufferCount;

        void Initialize() {
            Framebuffers = FrameBufferRequest.response->framebuffers;
            FramebufferCount = FrameBufferRequest.response->framebuffer_count;
            MainFramebuffer = Framebuffers[0];
        }
    }
}
