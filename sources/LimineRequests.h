#pragma once

#include <limine.h>

namespace Neon {
    extern uintptr_t HHDMOffset;

    volatile inline limine_terminal_request TerminalRequest {
        .id = LIMINE_TERMINAL_REQUEST,
        .revision = 0,
        .response = nullptr,
        .callback = nullptr
    };

    volatile inline limine_framebuffer_request FrameBufferRequest {
        .id = LIMINE_FRAMEBUFFER_REQUEST,
        .revision = 0,
        .response = nullptr
    };

    volatile inline limine_hhdm_request HHDMRequest {
        .id = LIMINE_HHDM_REQUEST,
        .revision = 0,
        .response = nullptr
    };

    volatile inline limine_stack_size_request StackSizeRequest {
        .id = LIMINE_STACK_SIZE_REQUEST,
        .revision = 0,
        .response = nullptr,
        .stack_size = 0x10000
    };

    volatile inline limine_memmap_request MemoryMapRequest {
        .id = LIMINE_MEMMAP_REQUEST,
        .revision = 0,
        .response = nullptr
    };

    volatile inline limine_kernel_file_request KernelFileRequest {
        .id = LIMINE_KERNEL_FILE_REQUEST,
        .revision = 0,
        .response = nullptr
    };

    volatile inline limine_kernel_address_request KernelAddressRequest{
        .id = LIMINE_KERNEL_ADDRESS_REQUEST,
        .revision = 0,
        .response = nullptr
    };

    volatile inline limine_smp_request SMPRequest {
        .id = LIMINE_SMP_REQUEST,
        .revision = 0,
        .response = nullptr,
        #ifdef __x86_64__
        .flags = LIMINE_SMP_X2APIC
        #else
        .flags = 0
        #endif
    };

    volatile inline limine_boot_time_request BootTimeRequest {
        .id = LIMINE_BOOT_TIME_REQUEST,
        .revision = 0,
        .response = nullptr
    };
}