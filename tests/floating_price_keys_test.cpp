#include <cassert>
#include <limits>
#include "flat_hash_map.hpp"
#include "bytell_hash_map.hpp"
#include "unordered_map.hpp"

template<typename Map>
void test_normalized_floating_point_keys()
{
    Map map;

    map[0.1 + 0.2] = 1;
    map[0.3] = 2;
    assert(map.size() == 1);
    assert(map.find(0.3) != map.end());
    assert(map.find(0.1 + 0.2) != map.end());

    map.clear();
    map[0.3000001] = 1;
    map[0.3] = 2;
    assert(map.size() == 2);

    map.clear();
    map[0.0] = 1;
    map[-0.0] = 2;
    assert(map.size() == 1);

    map.clear();
    double nan = std::numeric_limits<double>::quiet_NaN();
    map[nan] = 1;
    map[nan] = 2;
    assert(map.size() == 1);
}

int main()
{
    test_normalized_floating_point_keys<ska::flat_hash_map<double, int>>();
    test_normalized_floating_point_keys<ska::bytell_hash_map<double, int>>();
    test_normalized_floating_point_keys<ska::unordered_map<double, int>>();
    return 0;
}
