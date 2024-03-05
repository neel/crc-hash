// SPDX-FileCopyrightText: 2023 Neel Basu
// SPDX-License-Identifier: BSD-3-Clause

#ifndef NCH_ALGORITHMS_H_71704c00
#define NCH_ALGORITHMS_H_71704c00

#include <climits>
#include <cstddef>
#include <cstdint>
#include <boost/crc.hpp>

namespace noch{

namespace algorithms{

template <std::size_t Bits, unsigned long TruncPoly, unsigned long InitRem, unsigned long FinalXor, bool ReflectIn, bool ReflectRem>
struct crc{
    using hash_type  = boost::crc_optimal<Bits, TruncPoly, InitRem, FinalXor, ReflectIn, ReflectRem>;
    using value_type = typename hash_type::value_type;

    static const value_type init = hash_type::initial_remainder;

    struct state{
        using algorithm = crc<Bits, TruncPoly, InitRem, FinalXor, ReflectIn, ReflectRem>;

        using value = value_type;

        value _seed;

        inline state(): _seed(init) {}
        inline state(const value& v): _seed(v) {}
        inline state(const state& other) = default;

        inline value seed() const { return _seed; }

        inline state& operator=(const value& v){
            _seed = v;
            return *this;
        }
        inline operator value() const { return seed(); }

        value compute(const std::uint8_t* ptr, std::size_t bytes) {
            hash_type result(_seed);
            result.process_bytes(ptr, bytes);
            value res = result.checksum();
            _seed = result.get_interim_remainder();
            return res;
        }

        value operator()(const std::uint8_t* ptr, std::size_t bytes) {
            return compute(ptr, bytes);
        }
    };
};

using crc_16_type       = crc<16, 0x8005, 0, 0, true, true>;
using crc_ccitt_false_t = crc<16, 0x1021, 0xFFFF, 0, false, false>;
using crc_ccitt_type    = crc_ccitt_false_t;
using crc_ccitt_true_t  = crc<16, 0x1021, 0, 0, true, true>;
using crc_xmodem_type   = crc<16, 0x8408, 0, 0, true, true>;
using crc_xmodem_t      = crc<16, 0x1021, 0, 0, false, false>;
using crc_32_type       = crc<32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true>;
using crc_64_iso_type   = crc<64, 0x000000000000001B, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, true, true>;
using crc_64_ecma_type  = crc<64, 0x42F0E1EBA9EA3693, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, true, true>;

namespace detail{
    template <std::size_t Bits>
    struct fnv1a_params;

    template <>
    struct fnv1a_params<32>{
        using value_type = std::uint32_t;

        static constexpr value_type prime = 16777619u;
        static constexpr value_type offset = 2166136261u;
    };

    template <>
    struct fnv1a_params<64>{
        using value_type = std::uint64_t;

        static constexpr value_type prime = 1099511628211u;
        static constexpr value_type offset = 14695981039346656037u;
    };

    template <std::size_t Bits>
    struct fnv1a{
        using params_type = fnv1a_params<Bits>;
        using value_type  = typename params_type::value_type;

        static value_type compute(const std::uint8_t* ptr, std::size_t length, value_type seed = params_type::offset){
            const std::uint8_t* begin = ptr;
            const std::uint8_t* end   = ptr + length;

            uint64_t hash = seed;
            for (const std::uint8_t* it = begin; it != end; ++it) {
                hash ^= *it;
                hash *= params_type::prime;
            }
            return hash;
        }
    };

}

template <std::size_t Bits>
struct fnv1a{
    using params_type = typename detail::fnv1a_params<Bits>;
    using value_type  = typename params_type::value_type;

    static const value_type init = params_type::offset;

    struct state{
        using algorithm = fnv1a<Bits>;

        using value = value_type;

        value _seed;

        inline state(): _seed(init) {}
        inline state(const value& v): _seed(v) {}
        inline state(const state& other) = default;

        inline value seed() const { return _seed; }

        inline state& operator=(const value& v){
            _seed = v;
            return *this;
        }
        inline operator value() const { return seed(); }

        value compute(const std::uint8_t* ptr, std::size_t bytes) {
            value res = detail::fnv1a<Bits>::compute(ptr, bytes, _seed);
            _seed = res;
            return res;
        }

        value operator()(const std::uint8_t* ptr, std::size_t bytes) {
            return compute(ptr, bytes);
        }
    };
};

using fnv1a32 = fnv1a<32>;
using fnv1a64 = fnv1a<64>;

namespace detail{

/**
 * @preserve
 * JS Implementation of incremental MurmurHash3 (r150) (as of May 10, 2013)
 *
 * Translated from Javascript https://github.com/jensyt/imurmurhash-js/blob/master/imurmurhash.js 234ad0c
 *
 * @author <a href="mailto:jensyt@gmail.com">Jens Taylor</a>
 * @see http://github.com/homebrewing/brauhaus-diff
 * @author <a href="mailto:gary.court@gmail.com">Gary Court</a>
 * @see http://github.com/garycourt/murmurhash-js
 * @author <a href="mailto:aappleby@gmail.com">Austin Appleby</a>
 * @see http://sites.google.com/site/murmurhash/
 */
class MurmurHash3 {
public:
    MurmurHash3(std::uint32_t seed = 0) : _seed(seed), _k1(0), _len(0), _rem(0) {}

    MurmurHash3& hash(const std::uint8_t* key, std::size_t len) {
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

    std::uint32_t result() {
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

    MurmurHash3& reset(uint32_t seed = 0) {
        _seed = seed;
        _rem = _k1 = _len = 0;
        return *this;
    }

    std::uint32_t seed() const { return _seed; }
private:
    std::uint32_t _seed, _k1, _len;
    std::uint8_t _rem;
};

}

template <std::size_t Bits>
struct imurmur;

template <>
struct imurmur<32>{
    using value_type  = std::uint32_t;

    static const value_type init = 0;

    struct state{
        using algorithm = imurmur<32>;

        using value = value_type;

        inline state(): _murmur(init) {}
        inline state(const value& v): _murmur(v) {}
        inline state(const state& other) = default;

        inline value seed() const { return _murmur.seed(); }

        // inline state& operator=(const value& v){
        //     _seed = v;
        //     return *this;
        // }

        inline operator value() const { return seed(); }

        value compute(const std::uint8_t* ptr, std::size_t bytes) {
            _murmur.hash(ptr, bytes);
            return _murmur.result();
        }

        value operator()(const std::uint8_t* ptr, std::size_t bytes) {
            return compute(ptr, bytes);
        }

        private:
            detail::MurmurHash3 _murmur;
    };
};

using imurmur32 = imurmur<32>;

}

}

#endif // NCH_ALGORITHMS_H_71704c00
