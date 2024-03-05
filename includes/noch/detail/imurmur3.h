#ifndef NCH_ALGORITHMS_DETAIL_IMURMUR3_H
#define NCH_ALGORITHMS_DETAIL_IMURMUR3_H

#include <cstdint>

namespace noch{
namespace algorithms{
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
class imurmur3 {
  public:
    explicit imurmur3(std::uint32_t seed = 0);
    imurmur3& hash(const std::uint8_t* key, std::size_t len);
    std::uint32_t result() const;
    imurmur3& reset(uint32_t seed = 0);
    inline std::uint32_t seed() const { return _seed; }
  private:
    std::uint32_t _seed, _k1, _len;
    std::uint8_t _rem;
};

}
}
}

#endif // NCH_ALGORITHMS_DETAIL_IMURMUR3_H
