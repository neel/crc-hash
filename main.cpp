#include <iostream>
#include <cstdint>
#include <vector>
#include <type_traits>
#include <boost/crc.hpp>
#include "crc.h"

namespace app{

template <typename T>
struct coordinates{
    using value_type = T;
    constexpr coordinates(value_type x, value_type y): _x(x), _y(y) {}
    static coordinates<T> zero() { return coordinates<T>{0, 0}; }

    constexpr value_type x() const { return _x; }
    constexpr value_type y() const { return _y; }

    friend crc::value_type hash_value(const coordinates<T>& coords, crc::value_type& seed){
        seed = crc::hash_values(seed, coords._x, coords._y);
        return seed;
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

    content(value_type angle): _angle(angle) {}

    void add(const coordinates_type& coords){
        _points.push_back(coords);
    }
    void add(const value_type& x, const value_type& y){
        _points.push_back(coordinates_type(x, y));
    }

    friend crc::value_type hash_value(const content<T>& c, crc::value_type& seed){
        seed = crc::hash_value(c._points, seed);
        seed = crc::hash_value(c._angle, seed);
        return seed;
    }

    private:
        container_type _points;
        value_type     _angle;
};

}

int main(int argc, char **argv) {
    app::content<int> c(42);
    c.add(10, 20);
    c.add(65, 46);

    std::cout << crc::hash_value(app::coordinates<double>{10, 20}) << " " << crc::hash_value(app::coordinates<int>{10, 20}) << std::endl;
    std::cout << crc::hash_value(c) << std::endl;

    crc::value_type seed = crc::init;

    std::cout << std::hex << crc::hash_value(std::string("Hello"), seed) << std::endl;
    std::cout << std::hex << crc::hash_value(std::string("Hello"), seed) << std::endl;

    return 0;
}
