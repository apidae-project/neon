#include <Common/Math.h>

namespace Neon {
    int pow(int Base, int Exponent) {
        int result = 1;
        while(Exponent > 0) {
            result = result * Base;
            Exponent--;
        }
        return result;
    }

    int abs(int Number) {
        return Number < 0 ? -Number : Number;
    }

    int sign(int Number) {
        return (Number > 0) ? 1 : ((Number < 0) ? -1 : 0);
    }
}