#include <Core/ELF.h>
#include <Services/Service.h>

namespace Neon
{
namespace Services
{
void Load(const char *Path) { ELF::Load(0, 0); }

void Unload(const char *Name) {}
} // namespace Services
} // namespace Neon