#pragma once

namespace Neon {
    template<class T>
    class Vector {
        T *Data;
    public:
        ~Vector() { delete[] Data; }
    };
}