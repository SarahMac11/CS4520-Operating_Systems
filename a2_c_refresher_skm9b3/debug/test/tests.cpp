#include "../include/debug.h"

#include <cstdint>
#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

TEST(terrible_sort_tests, badParam) {
    {
        // expected = given, because these shouldn't modify the data
        std::vector<uint16_t> given = {42, 16, 8, 15, 4}, expected(given);

        ASSERT_FALSE(terrible_sort(NULL, given.size()));

        ASSERT_FALSE(terrible_sort(given.data(), 0));
        ASSERT_TRUE(std::equal(given.cbegin(), given.cend(), expected.cbegin()));
    }
    {
        std::vector<uint16_t> given;
        ASSERT_FALSE(terrible_sort(given.data(), given.size()));
    }
}

TEST(terrible_sort_tests, generalUse) {
    {
        std::vector<uint16_t> given = {42, 16, 8, 15, 4}, expected(given);
        std::sort(expected.begin(), expected.end());

        ASSERT_TRUE(terrible_sort(given.data(), given.size()));
        ASSERT_TRUE(std::equal(given.cbegin(), given.cend(), expected.cbegin()));
    }
    {
        std::vector<uint16_t> given = {7}, expected(given);
        std::sort(expected.begin(), expected.end());

        ASSERT_TRUE(terrible_sort(given.data(), given.size()));
        ASSERT_TRUE(std::equal(given.cbegin(), given.cend(), expected.cbegin()));
    }
    {
        std::vector<uint16_t> given = {42, 16, 8, 15, 4, 42, 16, 8, 15, 4}, expected(given);
        std::sort(expected.begin(), expected.end());

        ASSERT_TRUE(terrible_sort(given.data(), given.size()));
        ASSERT_TRUE(std::equal(given.cbegin(), given.cend(), expected.cbegin()));
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}






