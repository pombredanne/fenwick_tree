#ifndef __TYPED_FENWICK_TREE_TEST_H__
#define __TYPED_FENWICK_TREE_TEST_H__

#include <gtest/gtest.h>
#include <cstdint>
#include "./test_utils.hpp"

extern std::uint64_t inc1[];
extern std::uint64_t inc2[];

TEST(typed_fenwick_tree, increments_by_one)
{
    // tree8
    // heigth          2 |                                   1 |                                                                       0
    // node  14       13 |       12       11       10        9 |        8        7        6        5       4         3        2        1
    // 00000100 00000100 | 00000010 00000010 00000010 00000010 | 00000001 00000001 00000001 00000001 00000001 00000001 00000001 00000001

    // tree16
    // heigth         3
    // node          15
    // 0000000000001000

    TypedFenwickTree_Test t(inc1, 15);

    // bit_count
    EXPECT_EQ(8*14 + 16, t.bit_count());

    std::string tree8_str = "0000010000000100000000100000001000000010000000100000000100000001000000010000000100000001000000010000000100000001";
    std::string tree16_str = "0000000000001000";
    EXPECT_EQ(tree8_str, tree_tostring(t.tree8, 8*8 + 8*4 + 8*2));
    EXPECT_EQ(tree16_str, tree_tostring(reinterpret_cast<std::uint8_t*>(t.tree16), 16*1));

    std::uint64_t seq1[15];
    increments_to_sequence(inc1, seq1, 15);

    // get
    for (size_t i = 0; i < 15; i++)
        EXPECT_EQ(seq1[i], t.get(i)) << "at index " << i;

    // find
    for (std::uint64_t i = 0; i < 15; i++)
        EXPECT_EQ(i, t.find(seq1[i])) << "at index " << i;
    for (std::uint64_t i = seq1[14]; i < 100; i++)
        EXPECT_EQ(14, t.find(i)) << "at index " << i;

    // set
    for (size_t i = 0; i < 15; i++)
        t.set(i, i);

    size_t sum = 0;
    for (size_t i = 0; i < 15; i++) {
        sum += i;
        EXPECT_EQ(seq1[i]+sum, t.get(i)) << "at index " << i;
    }

    // find
    sum = 0;
    for (std::uint64_t i = 0; i < 15; i++) {
        sum += i;
        EXPECT_EQ(i, t.find(seq1[i]+sum)) << "at index " << i;
    }
}


TEST(typed_fenwick_tree, increasing_increments)
{
    // tree8
    // heigth          2 |                                   1 |                                                                       0
    // node  14       13 |       12       11       10        9 |        8        7        6        5       4         3        2        1
    // 00101010 00001010 | 00011011 00010011 00001011 00000011 | 00001111 00001101 00001011 00001001 00000111 00000101 00000011 00000001

    // tree16
    // heigth         3
    // node          15
    // 0000000000100100

    TypedFenwickTree_Test t(inc2, 15);

    // bit_count
    EXPECT_EQ(8*14 + 16, t.bit_count());

    std::string tree8_str = "0010101000001010000110110001001100001011000000110000111100001101000010110000100100000111000001010000001100000001";
    std::string tree16_str = "0000000000100100";
    EXPECT_EQ(tree8_str, tree_tostring(t.tree8, 8*8 + 8*4 + 8*2));
    EXPECT_EQ(tree16_str, tree_tostring(reinterpret_cast<std::uint8_t*>(t.tree16), 16*1));

    std::uint64_t seq2[15];
    increments_to_sequence(inc2, seq2, 15);

    // get
    for (size_t i = 0; i < 15; i++)
        EXPECT_EQ(seq2[i], t.get(i)) << "at index " << i;

    // find
    for (std::uint64_t i = 0; i < 15; i++)
        EXPECT_EQ(i, t.find(seq2[i])) << "at index " << i;
    for (std::uint64_t i = seq2[14]; i < 1000; i++)
        EXPECT_EQ(14, t.find(i)) << "at index " << i;

    // set
    for (size_t i = 0; i < 15; i++)
        t.set(i, i);

    size_t sum = 0;
    for (size_t i = 0; i < 15; i++) {
        sum += i;
        EXPECT_EQ(seq2[i]+sum, t.get(i)) << "at index " << i;
    }

    // find
    sum = 0;
    for (std::uint64_t i = 0; i < 15; i++) {
        sum += i;
        EXPECT_EQ(i, t.find(seq2[i]+sum)) << "at index " << i;
    }
}

#endif // __TYPED_FENWICK_TREE_TEST_H__
