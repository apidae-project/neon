#include <Common/Math.h>

namespace Neon {
    int pow(int Base, int Exponent) {
        int result = 1;
        for(Exponent; Exponent > 0; Exponent--) {
            result = result * Base;
        }
        return result;
    }

    int abs(int num) {
        return num < 0 ? -num : num;
    }

    int sign(int num) {
        return (num > 0) ? 1 : ((num < 0) ? -1 : 0);
    }
}