// SPDX-FileCopyrightText: 2023 Neel Basu
// SPDX-License-Identifier: BSD-3-Clause

#ifndef NCH_ALGORITHMS_FNV_H
#define NCH_ALGORITHMS_FNV_H

#include <climits>
#include <cstddef>
#include <cstdint>

namespace noch{
namespace algorithms{

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


}
}

#endif // NCH_ALGORITHMS_FNV_H
