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
struct hash_combine_internal;

template <typename T>
struct hash_combine_internal<T, false>{
    template<typename State>
    static typename State::value compute(State& state, const T& v){
        static_assert(std::is_arithmetic_v<T>);
        return compute(state, &v, sizeof(T));
    }
    template<typename State>
    static typename State::value compute(State& state, const T* ptr, std::size_t bytes){
        const std::uint8_t* raw = reinterpret_cast<const std::uint8_t*>(ptr);
        return state(raw, bytes);
    }
};

template <typename T>
struct hash_combine_internal<T, true>{
    template<typename State>
    static typename State::value compute(State& state, const T& v){
        return noch::hash<T>{}(state, v);
    }
    template<typename State>
    static typename State::value compute(State& state, const T* ptr, std::size_t bytes){
        return noch::hash<T>{}(state, ptr, bytes);
    }
};

template <typename T, typename State>
typename State::value hash_value(State& state, const T& v){
    return hash_combine_internal<T, specialized<T>::value>::compute(state, v);
}

template <typename T, typename State>
typename State::value hash_combine(State& state, const T& v){
    return hash_value(state, v);
}

template <typename T, typename State>
typename State::value hash_combine(State& state, const T* ptr, std::size_t size){
    return hash_combine_internal<T, specialized<T>::value>::compute(state, ptr, size * sizeof(T));
}

template <typename State, typename X, typename... T>
typename State::value hash_combine(State& state, const X& x, const T&... vs){
           hash_combine(state, x);
    return hash_combine(state, vs...);
}



template <typename T>
struct hasher{
    template<typename State>
    typename State::value operator()(State& state, const T& v) const{
        return static_cast<const hash<T>*>(this)->combine(state, v);
    }
};

template <typename U, typename V>
struct hash<std::pair<U, V>>: hasher<std::pair<U, V>>{
    template<typename State>
    typename State::value combine(State& state, const std::pair<U, V>& pair) const {
               hash_combine(state, pair.first);
        return hash_combine(state, pair.second);
    }
};

template <typename T>
struct hash<std::vector<T>>: hasher<std::vector<T>>{
    template<typename State>
    typename State::value combine(State& state, const std::vector<T>& vec) const {
        typename State::value res = 0;
        for(const T& v: vec)
            res = hash_combine(state, v);
        return res;
    }
};

template <typename T>
struct hash<std::basic_string<T>>: hasher<std::basic_string<T>>{
    template<typename State>
    typename State::value combine(State& state, const std::basic_string<T>& str) const {
        typename State::value res = hash_combine(state, str.data(), str.size());
        return res;
    }
};

template <typename Algorithm>
struct algorithm{
    using state = typename Algorithm::state;
    using value = typename state::value;

    template <typename T>
    static value compute(state& s, const T& v){
        return hash_combine(s, v);
    }

    template <typename T>
    static value compute(const T& v){
        state s;
        return algorithm<Algorithm>::compute(s, v);
    }
};

template <typename Algorithm, typename T>
typename Algorithm::state::value hash_value(typename Algorithm::state& state, const T& v){
    return hash_combine(state, v);
}

template <typename Algorithm, typename T>
typename Algorithm::state::value hash_value(const T& v){
    typename Algorithm::state init;
    return hash_value(init, v);
}

}

#endif // NCH_HASH_H
