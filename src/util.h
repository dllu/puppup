#pragma once
#include <array>
#include <iostream>
#include <locale>
#include <string>

using idx = std::ptrdiff_t;
using chr = idx;

namespace puppup {
constexpr chr emptiness = 27;
constexpr chr blank = 26;
constexpr char idxstr[] = "eiaontrsuldgywvpmhfcbzxqkj_    +";
constexpr char idxSTR[] = "EIAONTRSULDGYWVPMHFCBZXQKJ_    +";
constexpr char ABC[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr char abc[] = "abcdefghijklmnopqrstuvwxyz";
using Rack = std::array<idx, 32>;
constexpr Rack scores{{1, 1, 1, 1, 1, 1,  1, 1,  1, 1, 2, 2, 4, 4, 4, 3,
                       3, 4, 4, 3, 3, 10, 8, 10, 5, 8, 0, 0, 0, 0, 0, 0}};
constexpr Rack starting_population{{12, 9, 9, 8, 6, 6, 6, 4, 4, 4, 4,
                                    3,  2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
                                    1,  1, 1, 1, 2, 0, 0, 0, 0, 0}};

constexpr idx stridx[] = {
    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
    30, 30, 30, 30, 30, 31, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
    26, 30, 2,  20, 19, 10, 0,  18, 11, 17, 1,  25, 24, 9,  16, 4,  3,  15, 23,
    6,  7,  5,  8,  14, 13, 22, 12, 21, 30, 30, 30, 30, 30};

template <idx N>
std::array<idx, N> constArray(idx x) {
    std::array<idx, N> a;
    a.fill(x);
    return a;
}

const Rack empty_rack = constArray<32>(0);

inline void print(const Rack& rack) {
    for (chr i = 0; i < 32; ++i) {
        for (idx j = 0; j < rack[i]; ++j) {
            std::cout << idxstr[i];
        }
    }
    for (chr i = 0; i < 32; ++i) {
        std::cout << rack[i] << " ";
    }
    std::cout << std::endl;
}
}  // namespace puppup
