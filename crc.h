// SPDX-FileCopyrightText: 2023 Neel Basu
// SPDX-License-Identifier: BSD-3-Clause

#ifndef CRC_HASH_H_71704c00
#define CRC_HASH_H_71704c00

#include <vector>
#include <cstdint>
#include <type_traits>
#include <boost/crc.hpp>

namespace crc{

// using hash_type  = boost::crc_optimal<64, 0x42F0E1EBA9EA3693, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, false, false>; // boost::crc_32_type;
using hash_type  = boost::crc_32_type;
using value_type = hash_type::value_type;

static const value_type init = hash_type::initial_remainder;

template <typename T>
struct hash;

template <typename T>
class specialized{
    template <class Arg, class Dummy = decltype(crc::hash<Arg>{})>
    static constexpr bool exists(int) { return true; }
    template <class Arg>
    static constexpr bool exists(char) { return false; }
    public:
        static constexpr bool value = exists<T>(42);
};

template <typename T, bool Specialized>
struct hash_value_internal;

template <typename T>
struct hash_value_internal<T, false>{
    static value_type compute(const T& v, value_type& seed){
        return compute(&v, sizeof(T), seed);
    }
    static value_type compute(const T* ptr, std::size_t bytes, value_type& seed){
        hash_type result(seed);
        result.process_bytes(reinterpret_cast<const std::uint8_t*>(ptr), bytes);
        value_type res = result.checksum();
        seed = result.get_interim_remainder();
        return res;
    }
};

template <typename T>
struct hash_value_internal<T, true>{
    static value_type compute(const T& v, value_type& seed){
        return crc::hash<T>{}(v, seed);
    }
    static value_type compute(const T* ptr, std::size_t bytes, value_type& seed){
        return crc::hash<T>{}(ptr, bytes, seed);
    }
};


template <typename T>
value_type hash_value(const T& v, value_type& seed){
    return hash_value_internal<T, specialized<T>::value>::compute(v, seed);
}

template <typename T>
value_type hash_value(const T* ptr, std::size_t size, value_type& seed){
    return hash_value_internal<T, specialized<T>::value>::compute(ptr, size * sizeof(T), seed);
}

template <typename... T>
value_type hash_values(value_type& seed, const T&... vs){
    value_type res = 0;
    ([&]{ res = hash_value(vs, seed); }(), ...);
    return res;
}



template <typename T>
struct hasher{
    value_type operator()(const T& v, value_type& seed) const{
        return static_cast<const hash<T>*>(this)->combine(v, seed);
    }
};

template <typename T>
struct hash<std::vector<T>>: hasher<std::vector<T>>{
    value_type combine(const std::vector<T>& vec, value_type& seed) const {
        value_type res = 0;
        for(const T& v: vec)
            res = hash_value(v, seed);
        return res;
    }
};

template <typename T>
struct hash<std::basic_string<T>>: hasher<std::basic_string<T>>{
    value_type combine(const std::basic_string<T>& str, value_type& seed) const {
        value_type res = hash_value(str.data(), str.size(), seed);
        return res;
    }
};

template <typename T>
value_type hash_value(const T& v){
    value_type seed = init;
    return hash_value(v, seed);
}

}

#endif // CRC_HASH_H_71704c00
