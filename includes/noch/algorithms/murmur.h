// SPDX-FileCopyrightText: 2023 Neel Basu
// SPDX-License-Identifier: BSD-3-Clause

#ifndef NCH_ALGORITHMS_MURMUR_FNV_H
#define NCH_ALGORITHMS_MURMUR_FNV_H

#include <climits>
#include <cstddef>
#include <cstdint>
#include <noch/detail/imurmur3.h>

namespace noch{
namespace algorithms{

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
            noch::algorithms::detail::imurmur3 _murmur;
    };
};

using imurmur32 = imurmur<32>;

}
}

#endif // NCH_ALGORITHMS_MURMUR_FNV_H
