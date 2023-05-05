#ifdef __x86_64__
#include <Amd64/Amd64.h>
#endif

namespace Neon
{
namespace HAL
{
void Halt()
{
#ifdef __x86_64__
	asm("hlt");
#endif
}

void ClearInterrupts()
{
#ifdef __x86_64__
	asm("cli");
#endif
}

void Pause()
{
#ifdef __x86_64__
	asm("pause");
#endif
}
} // namespace HAL
} // namespace Neon
