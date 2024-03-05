#include "noch/detail/imurmur3.h"

noch::algorithms::detail::imurmur3::imurmur3(std::uint32_t seed): _seed(seed), _k1(0), _len(0), _rem(0) {}

noch::algorithms::detail::imurmur3& noch::algorithms::detail::imurmur3::hash(const std::uint8_t* key, std::size_t len) {
    std::uint32_t k1 = _k1, top;

    _len += len;

    std::size_t i = 0;

    switch (_rem) {
        case 0:
            k1 ^= len > i ? (*(key + i++) & 0xffff) : 0;
        case 1:
            k1 ^= len > i ? (*(key + i++) & 0xffff) << 8  : 0;
        case 2:
            k1 ^= len > i ? (*(key + i++) & 0xffff) << 16 : 0;
        case 3:
            k1 ^= len > i ? (*(key + i)   & 0xff)   << 24 : 0;
            k1 ^= len > i ? (*(key + i++) & 0xff00) >> 8  : 0;
    }

    _rem = (len + _rem) & 3; // & 3 is same as % 4
    len -= _rem;

    if (len > 0) {
        std::uint32_t seed = _seed;
        while (true) {
            k1 = (k1 * 0x2d51 + (k1 & 0xffff) * 0xcc9e0000) & 0xffffffff;
            k1 = (k1 << 15) | (k1 >> 17);
            k1 = (k1 * 0x3593 + (k1 & 0xffff) * 0x1b870000) & 0xffffffff;

            seed ^= k1;
            seed = (seed << 13) | (seed >> 19);
            seed = (seed * 5 + 0xe6546b64) & 0xffffffff;

            if (i >= len) {
                break;
            }
            k1 = ((*(key + i++) & 0xffff)) ^
                    ((*(key + i++) & 0xffff) << 8) ^
                    ((*(key + i++) & 0xffff) << 16);
            top = *(key + i++);
            k1 ^= ((top & 0xff) << 24) ^
                    ((top & 0xff00) >> 8);
        }
        k1 = 0;
        switch (_rem) {
            case 3: k1 ^= (key[i + 2] & 0xffff) << 16;
            case 2: k1 ^= (key[i + 1] & 0xffff) << 8;
            case 1: k1 ^= (*(key + i) & 0xffff);
        }
        _seed = seed;
    }
    _k1 = k1;
    return *this;
}

std::uint32_t noch::algorithms::detail::imurmur3::result() const {
    std::uint32_t k1 = _k1;
    std::uint32_t h1 = _seed;

    if (k1 > 0) {
        k1 = (k1 * 0x2d51 + (k1 & 0xffff) * 0xcc9e0000) & 0xffffffff;
        k1 = (k1 << 15) | (k1 >> 17);
        k1 = (k1 * 0x3593 + (k1 & 0xffff) * 0x1b870000) & 0xffffffff;
        h1 ^= k1;
    }

    h1 ^= _len;

    h1 ^= h1 >> 16;
    h1 = (h1 * 0xca6b + (h1 & 0xffff) * 0x85eb0000) & 0xffffffff;
    h1 ^= h1 >> 13;
    h1 = (h1 * 0xae35 + (h1 & 0xffff) * 0xc2b20000) & 0xffffffff;
    h1 ^= h1 >> 16;

    return h1 >> 0;
}

noch::algorithms::detail::imurmur3& noch::algorithms::detail::imurmur3::reset(uint32_t seed){
    _seed = seed;
    _rem = _k1 = _len = 0;
    return *this;
}

