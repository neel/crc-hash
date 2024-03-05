// SPDX-FileCopyrightText: 2023 Neel Basu
// SPDX-License-Identifier: BSD-3-Clause

#ifndef NCH_ALGORITHMS_MIXED_H
#define NCH_ALGORITHMS_MIXED_H

#include <climits>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <noch/algorithms/crc.h>
#include <noch/algorithms/fnv.h>
#include <noch/algorithms/murmur.h>

#ifdef __SIZEOF_INT128__
using uint128_t = unsigned __int128;
std::ostream& operator<<(std::ostream& stream, uint128_t value){
    uint64_t upper = static_cast<uint64_t>(value >> 64);
    uint64_t lower = static_cast<uint64_t>(value);

    std::ios_base::fmtflags f(stream.flags());
    stream << std::hex << upper << lower;
    stream.flags(f);
    return stream;
}
#else
#include <boost/multiprecision/cpp_int.hpp>
using uint128_t = boost::multiprecision::uint128_t;
#endif


namespace noch{
namespace algorithms{


template <typename Accumulator, typename... Algo>
struct multi_hash{
    using tuple_value_type = std::tuple<typename Algo::value_type...>;
    using states_type = std::tuple<typename Algo::state...>;

    struct state{
        using algorithm = multi_hash<Algo...>;
        using value = typename Accumulator::result_type;

        inline state() {}
        inline state(const state& other) = default;

        value compute(const std::uint8_t* ptr, std::size_t bytes) {
            tuple_value_type res = std::apply([&](typename Algo::state&... state){
                return std::make_tuple(state(ptr, bytes)...);
            }, _states);
            return Accumulator::accumulate(res);
        }

        value operator()(const std::uint8_t* ptr, std::size_t bytes) {
            return compute(ptr, bytes);
        }

        private:
            states_type _states;

    };
};

namespace detail{

    struct mixed1{
        struct combinator{
            using result_type = uint128_t;

            static result_type accumulate(const std::tuple<std::uint64_t, std::uint32_t, std::uint32_t>& input){
                // std::cout << "pieces: " << std::hex << std::get<0>(input) << " " << std::hex << std::get<1>(input) << " " << std::hex << std::get<2>(input) << std::endl;
                result_type result = std::get<0>(input);
                result <<= 64;
                result += (static_cast<result_type>(std::get<1>(input)) << 32);
                result += std::get<2>(input);
                return result;
            }
        };

        using algo = multi_hash<combinator, noch::algorithms::crc64_ecma, noch::algorithms::fnv1a32, noch::algorithms::imurmur32>;
    };

}

using mixed1 = detail::mixed1::algo;

}
}

#endif // NCH_ALGORITHMS_MIXED_H
