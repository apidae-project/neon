#pragma once

#include <Cxxshim/cstdint>

static constexpr uint64_t ATEXIT_MAX_FUNCS = 128;

extern "C" {
    typedef unsigned uarch_t;

    struct atexit_func_entry_t {
        void (*destructor_func)(void*);
        void *obj_ptr;
        void *dso_handle;
    };
}

namespace cxxabi {
    void init();
}