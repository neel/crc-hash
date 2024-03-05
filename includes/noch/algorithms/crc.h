// SPDX-FileCopyrightText: 2023 Neel Basu
// SPDX-License-Identifier: BSD-3-Clause

#ifndef NCH_ALGORITHMS_CRC_H
#define NCH_ALGORITHMS_CRC_H

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

using crc16             = crc<16, 0x8005, 0, 0, true, true>;
using crc_ccitt_false   = crc<16, 0x1021, 0xFFFF, 0, false, false>;
using crc_ccitt_true    = crc<16, 0x1021, 0, 0, true, true>;
using crc_xmodem        = crc<16, 0x8408, 0, 0, true, true>;
using crc_xmodem_false  = crc<16, 0x1021, 0, 0, false, false>;
using crc32             = crc<32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true>;
using crc64_iso         = crc<64, 0x000000000000001B, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, true, true>;
using crc64_ecma        = crc<64, 0x42F0E1EBA9EA3693, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, true, true>;

}
}

#endif // NCH_ALGORITHMS_CRC_H
