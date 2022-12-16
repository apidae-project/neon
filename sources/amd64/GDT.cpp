#include <amd64/Amd64.h>
#include <Common/Log.h>
#include <Common/Lock.h>
#include <Cxxshim/new>

namespace Neon {
    namespace Amd64 {
        namespace GDT {
            TSS *tss = nullptr;
            static lock_t lock;

            void Initialize(void) {
                lockit(lock);
                Log("Starting GDT..");
                uintptr_t base = reinterpret_cast<uintptr_t>(&tss[0]);
                uint16_t limit = sizeof(tss[0]);
                asm volatile (
                    "mov %[dsel], %%ds \n\t"
                    "mov %[dsel], %%fs \n\t"
                    "mov %[dsel], %%gs \n\t"
                    "mov %[dsel], %%es \n\t"
                    "mov %[dsel], %%ss \n\t"
                    :: [dsel]"m"(GDT_DATA)
                );
                asm volatile (
                    "push %[csel] \n\t"
                    "lea 1f(%%rip), %%rax \n\t"
                    "push %%rax \n\t"
                    ".byte 0x48, 0xCB \n\t"
                    "1:"
                    :: [csel]"i"(GDT_CODE) : "rax"
                );
                asm volatile ("ltr %0" :: "r"(static_cast<uint16_t>(GDT_TSS)));
                Log("Done!");
            }
        }
    }
}