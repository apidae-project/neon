#pragma once

#include <KernelMain/LimineRequests.hpp>

volatile struct LimineTerminalRequest {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0,
}