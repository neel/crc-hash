// SPDX-FileCopyrightText: 2023 Neel Basu
// SPDX-License-Identifier: BSD-3-Clause

#ifndef NCH_HASH_H
#define NCH_HASH_H

#include <vector>
#include <cstdint>
#include <type_traits>
#include <boost/crc.hpp>

namespace noch{

template <typename T>
struct hash;

template <typename T>
class specialized{
    template <class Arg, class Dummy = decltype(noch::hash<Arg>{})>
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
    template<typename State>
    static typename State::value compute(const T& v, State& state){
        return compute(&v, sizeof(T), state);
    }
    template<typename State>
    static typename State::value compute(const T* ptr, std::size_t bytes, State& state){
        const std::uint8_t* raw = reinterpret_cast<const std::uint8_t*>(ptr);
        return state(raw, bytes);
    }
};

template <typename T>
struct hash_value_internal<T, true>{
    template<typename State>
    static typename State::value compute(const T& v, State& state){
        return noch::hash<T>{}(v, state);
    }
    template<typename State>
    static typename State::value compute(const T* ptr, std::size_t bytes, State& state){
        return noch::hash<T>{}(ptr, bytes, state);
    }
};


template <typename T, typename State>
typename State::value hash_value(const T& v, State& state){
    return hash_value_internal<T, specialized<T>::value>::compute(v, state);
}

template <typename T, typename State>
typename State::value hash_value(const T* ptr, std::size_t size, State& state){
    return hash_value_internal<T, specialized<T>::value>::compute(ptr, size * sizeof(T), state);
}

template <typename State, typename... T>
typename State::value hash_values(State& state, const T&... vs){
    typename State::value res = 0;
    ([&]{ res = hash_value(vs, state); }(), ...);
    return res;
}



template <typename T>
struct hasher{
    template<typename State>
    typename State::value operator()(const T& v, State& state) const{
        return static_cast<const hash<T>*>(this)->combine(v, state);
    }
};

template <typename T>
struct hash<std::vector<T>>: hasher<std::vector<T>>{
    template<typename State>
    typename State::value combine(const std::vector<T>& vec, State& state) const {
        typename State::value res = 0;
        for(const T& v: vec)
            res = hash_value(v, state);
        return res;
    }
};

template <typename T>
struct hash<std::basic_string<T>>: hasher<std::basic_string<T>>{
    template<typename State>
    typename State::value combine(const std::basic_string<T>& str, State& state) const {
        typename State::value res = hash_value(str.data(), str.size(), state);
        return res;
    }
};

template <typename Algorithm>
struct algorithm{
    using state = typename Algorithm::state;
    using value = typename state::value;

    template <typename T>
    value operator()(const T& v, state& s) const{
        return hash_value(v, s);
    }

    template <typename T>
    value operator()(const T& v) const{
        state s;
        return algorithm<Algorithm>::operator()(v, s);
    }
};

template <typename Algorithm>
struct hashing{
    using state = typename Algorithm::state;
    using value = typename state::value;
    static algorithm<Algorithm> hash;
};

template <typename Algorithm>
algorithm<Algorithm> hashing<Algorithm>::hash;


template <typename Algorithm, typename T>
typename Algorithm::state::value hash_value(const T& v, typename Algorithm::state& state){
    return hash_value(v, state);
}

template <typename Algorithm, typename T>
typename Algorithm::state::value hash_value(const T& v){
    typename Algorithm::state init;
    return hash_value(v, init);
}

}

#endif // NCH_HASH_H
