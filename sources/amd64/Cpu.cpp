#include <amd64/amd64.h>
#include <Common/Log.h>

namespace Neon {
    namespace Amd64 {
        uint64_t rdmsr(uint32_t msr) {
            int32_t edx, eax;
            asm volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(msr) : "memory");
            return (static_cast<uint64_t>(edx) << 32) | eax;
        }

        void wrmsr(uint32_t msr, uint64_t value) {
            uint32_t edx = value >> 32;
            uint32_t eax = static_cast<uint32_t>(value);
            asm volatile("wrmsr" :: "a"(eax), "d"(edx), "c"(msr) : "memory");
        }

        void SetKernelGS(uint64_t Address) {
            wrmsr(0xC0000102, Address);
        }

        uint64_t GetKernelGS() {
            return rdmsr(0xC0000102);
        }

        void SetGS(uint64_t Address) {
            wrmsr(0xC0000101, Address);
        }

        uint64_t GetGS() {
            return rdmsr(0xC0000101);
        }

        void SetFS(uint64_t Address) {
            wrmsr(0xC0000100, Address);
        }

        uint64_t GetFS() {
            return rdmsr(0xC0000100);
        }

        void wrxcr(uint32_t i, uint64_t value) {
            uint32_t edx = value >> 32;
            uint32_t eax = static_cast<uint32_t>(value);
            asm volatile ("xsetbv" :: "a"(eax), "d"(edx), "c"(i) : "memory");
        }

        static uint64_t rfbm = ~0ULL;
        static uint32_t rfbm_low = rfbm & 0xFFFFFFFF;
        static uint32_t rfbm_high = (rfbm >> 32) & 0xFFFFFFFF;

        void xsaveopt(uint8_t *region) {
            asm volatile("xsaveopt64 (%0)" :: "r"(region), "a"(rfbm_low), "d"(rfbm_high) : "memory");
        }

        void xsave(uint8_t *region) {
            asm volatile("xsaveq (%0)" :: "r"(region), "a"(rfbm_low), "d"(rfbm_high) : "memory");
        }

        void xrstor(uint8_t *region) {
            asm volatile("xrstorq (%0)" :: "r"(region), "a"(rfbm_low), "d"(rfbm_high) : "memory");
        }

        void fxsave(uint8_t *region) {
            asm volatile("fxsaveq (%0)" :: "r"(region) : "memory");
        }

        void fxrstor(uint8_t *region) {
            asm volatile("fxrstorq (%0)" :: "r"(region) : "memory");
        }

        void invlpg(uint64_t Addressess) {
            asm volatile("invlpg (%0)" :: "r"(Addressess));
        }

        void EnableSSE() {
            WriteCR(0, (ReadCR(0) & ~(1 << 2)) | (1 << 1));
            WriteCR(4, ReadCR(4) | (3 << 9));
        }

        void EnableSMEP() {
            uint32_t a = 0, b = 0, c = 0, d = 0;
            if (__get_cpuid(7, &a, &b, &c, &d)) {
                if (b & CPUID_SMEP) {
                    WriteCR(4, ReadCR(4) | (1 << 20));
                }
            }
        }

        void EnableSMAP() {
            uint32_t a = 0, b = 0, c = 0, d = 0;
            if (__get_cpuid(7, &a, &b, &c, &d)) {
                if (b & CPUID_SMAP) {
                WriteCR(4, ReadCR(4) | (1 << 21));
                asm volatile("clac");
                }
            }
        }

        void EnableUMIP() {
            uint32_t a = 0, b = 0, c = 0, d = 0;
            if (__get_cpuid(7, &a, &b, &c, &d)) {
                if (c & CPUID_UMIP) {
                    WriteCR(4, ReadCR(4) | (1 << 11));
                }
            }
        }

        void EnablePAT(){
            wrmsr(0x277, WriteBack | (Uncachable << 8) | (WriteCombining << 16));
        }

        void EnableCPUFeatures() {
            Log("Enabling x64 specific CPU features...");
            EnableSSE();
            EnableSMEP();
            EnableSMAP();
            EnableUMIP();
            EnablePAT();
            Log("Done!");
        }
    }
}