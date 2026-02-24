#include "flat_hash_map.hpp"
#include <iostream>
#include <cmath>
#include <string>
#include <cstdlib>

static int test_count = 0;
static int pass_count = 0;
static int fail_count = 0;

#define ASSERT_TRUE(expr, msg) do { \
    ++test_count; \
    if (expr) { ++pass_count; std::cout << "  PASS: " << msg << std::endl; } \
    else { ++fail_count; std::cout << "  FAIL: " << msg << std::endl; } \
} while(0)

// Test that two double keys that differ only by floating-point error
// are treated as the same key (within 8 decimal places of precision).
void test_same_key_after_rounding()
{
    std::cout << "[test_same_key_after_rounding]" << std::endl;
    ska::flat_hash_map<double, std::string> map;

    // 0.1 + 0.2 != 0.3 in IEEE 754, but they round to the same
    // integer when scaled by 1e8.
    map[0.1 + 0.2] = "a";
    ASSERT_TRUE(map.count(0.3) == 1, "0.1+0.2 and 0.3 are the same key");
    ASSERT_TRUE(map[0.3] == "a", "0.3 retrieves value set via 0.1+0.2");

    // 0.1 + 0.7 vs 0.8
    map[0.1 + 0.7] = "b";
    ASSERT_TRUE(map.count(0.8) == 1, "0.1+0.7 and 0.8 are the same key");
    ASSERT_TRUE(map[0.8] == "b", "0.8 retrieves value set via 0.1+0.7");

    // 0.3 + 0.6 vs 0.9
    map[0.3 + 0.6] = "c";
    ASSERT_TRUE(map.count(0.9) == 1, "0.3+0.6 and 0.9 are the same key");
    ASSERT_TRUE(map[0.9] == "c", "0.9 retrieves value set via 0.3+0.6");

    // 1.0 - 0.7 vs 0.3
    // Reuses the 0.3 bucket already inserted above.
    map[1.0 - 0.7] = "d";
    ASSERT_TRUE(map.count(0.3) == 1, "1.0-0.7 and 0.3 are the same key");
    ASSERT_TRUE(map[0.3] == "d", "0.3 retrieves value set via 1.0-0.7");

    // 0.1 * 3 vs 0.3
    map[0.1 * 3] = "e";
    ASSERT_TRUE(map.count(0.3) == 1, "0.1*3 and 0.3 are the same key");
    ASSERT_TRUE(map[0.3] == "e", "0.3 retrieves value set via 0.1*3");
}

// Test that values differing beyond the 8th decimal place are still
// considered the same key after rounding.
void test_precision_boundary()
{
    std::cout << "[test_precision_boundary]" << std::endl;
    ska::flat_hash_map<double, int> map;

    // 1.000000001 and 1.000000004 both round to 100000000 when
    // multiplied by 1e8 and rounded, so they should be the same key.
    map[1.000000001] = 1;
    ASSERT_TRUE(map.count(1.000000004) == 1,
                "1.000000001 and 1.000000004 are the same key (diff < 0.5 at 8th decimal)");

    // 1.000000006 scales to ~100000000.6 which rounds to 100000001,
    // so it should be a different key from the 100000000 bucket above.
    map[1.000000006] = 2;
    ASSERT_TRUE(map.count(1.000000009) == 1,
                "1.000000006 and 1.000000009 are the same key");
    ASSERT_TRUE(map.size() == 2,
                "map has exactly 2 entries for the two distinct rounded keys");
}

// Test negative doubles.
void test_negative_keys()
{
    std::cout << "[test_negative_keys]" << std::endl;
    ska::flat_hash_map<double, int> map;

    map[-0.1 - 0.2] = 10;
    ASSERT_TRUE(map.count(-0.3) == 1, "-0.1-0.2 and -0.3 are the same key");
    ASSERT_TRUE(map[-0.3] == 10, "-0.3 retrieves value set via -0.1-0.2");

    map[-0.3 - 0.6] = 20;
    ASSERT_TRUE(map.count(-0.9) == 1, "-0.3-0.6 and -0.9 are the same key");
    ASSERT_TRUE(map[-0.9] == 20, "-0.9 retrieves value set via -0.3-0.6");
}

// Test that special floating-point values are handled correctly.
void test_special_values()
{
    std::cout << "[test_special_values]" << std::endl;
    ska::flat_hash_map<double, int> map;

    double pos_inf = std::numeric_limits<double>::infinity();
    double neg_inf = -std::numeric_limits<double>::infinity();
    double nan_val = std::nan("");

    map[pos_inf] = 1;
    map[neg_inf] = 2;
    map[nan_val] = 3;

    ASSERT_TRUE(map.count(pos_inf) == 1, "+inf is a valid key");
    ASSERT_TRUE(map[pos_inf] == 1, "+inf retrieves correct value");
    ASSERT_TRUE(map.count(neg_inf) == 1, "-inf is a valid key");
    ASSERT_TRUE(map[neg_inf] == 2, "-inf retrieves correct value");
    ASSERT_TRUE(map.count(nan_val) == 1, "NaN is a valid key");
    ASSERT_TRUE(map[nan_val] == 3, "NaN retrieves correct value");
    ASSERT_TRUE(map.size() == 3, "map has 3 entries for +inf, -inf, NaN");
}

// Test with a larger set of classic floating-point pitfalls.
void test_multiple_arithmetic_combinations()
{
    std::cout << "[test_multiple_arithmetic_combinations]" << std::endl;
    ska::flat_hash_map<double, int> map;

    // All of these should map to the key for 0.3 (30000000 after scaling by 1e8).
    double keys[] = {0.3, 0.1 + 0.2, 1.0 - 0.7, 0.1 * 3, 0.6 / 2.0};

    map[0.3] = 42;
    for (double k : keys)
    {
        ASSERT_TRUE(map.count(k) == 1,
                    "arithmetic expression maps to 0.3 key");
        ASSERT_TRUE(map[k] == 42,
                    "arithmetic expression retrieves value 42");
    }
    ASSERT_TRUE(map.size() == 1, "all expressions share a single entry");
}

int main()
{
    test_same_key_after_rounding();
    test_precision_boundary();
    test_negative_keys();
    test_special_values();
    test_multiple_arithmetic_combinations();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << pass_count << " passed, "
              << fail_count << " failed, "
              << test_count << " total" << std::endl;
    std::cout << "========================================" << std::endl;

    return fail_count == 0 ? 0 : 1;
}
