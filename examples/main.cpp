#include <iostream>
#include <cstdint>
#include <vector>
#include <type_traits>
#include <boost/crc.hpp>
#include <noch/hash.h>
#include <noch/algorithms.h>

namespace app{
namespace manifest{

using hash = noch::algorithm<noch::algorithms::mixed1>;

template <typename T>
struct coordinates{
    using value_type = T;
    constexpr coordinates(value_type x, value_type y): _x(x), _y(y) {}
    static coordinates<T> zero() { return coordinates<T>{0, 0}; }

    constexpr value_type x() const { return _x; }
    constexpr value_type y() const { return _y; }

    friend hash::value hash_value(hash::state& state, const coordinates<T>& coords){
        return noch::hash_combine(state, coords._x, coords._y);
    }

    private:
        value_type _x, _y;

    private:
        coordinates() = default;
};

template <typename T>
struct content{
    using value_type       = T;
    using coordinates_type = coordinates<value_type>;
    using container_type   = std::vector<coordinates_type>;

    content(value_type angle): _center(0, 0), _angle(angle) {}

    void add(const coordinates_type& coords){
        _points.push_back(coords);
    }
    void add(const value_type& x, const value_type& y){
        _points.push_back(coordinates_type(x, y));
    }

    friend hash::value hash_value(hash::state& state, const content<T>& c){
               noch::hash_combine(state, c._points);
               noch::hash_combine(state, c._center);
        return noch::hash_combine(state, c._angle);
    }

    private:
        container_type _points;
        coordinates<T> _center;
        value_type     _angle;
};

}
}


int main(int argc, char **argv) {
    app::manifest::content<int> c(42);
    c.add(10, 20);
    c.add(65, 46);

    std::cout << std::hex << app::manifest::hash::compute(app::manifest::coordinates<double>{10, 20}) << std::endl;
    std::cout << std::hex << app::manifest::hash::compute(app::manifest::coordinates<int>{10, 20}) << std::endl;
    std::cout << std::hex << app::manifest::hash::compute(c) << std::endl;

    app::manifest::hash::state state;
    std::cout << std::hex << app::manifest::hash::compute(state, std::string("this is a")) << std::endl;
    std::cout << std::hex << noch::hash_value<app::manifest::hash>(state, std::string(" test string")) << std::endl;

    return 0;
}
