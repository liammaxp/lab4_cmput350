#include <cstdint>
#include <cassert>


inline uint64_t expand(uint64_t input, uint32_t scale) {
    assert(scale >= 1);
    uint64_t result = 0;
    for (uint32_t i = 0; i < 64; ++i){
        if (static_cast<uint64_t>(i) * scale >=64){
            break;
        }
        if (input & (1ULL << i)){
            result |= (1ULL << (i * scale));
        }
    }
    return result;  // replace this with your impl!
}
inline uint64_t morton3d(uint64_t x, uint64_t y, uint64_t z){
    return expand(x, 3) | (expand(y, 3) << 1) | (expand(z, 3) << 2);
}
