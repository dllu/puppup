#pragma once
#include <array>
#include <string>

using idx = std::ptrdiff_t;
namespace puppup {
constexpr idx gaddag_marker = 31;
constexpr idx emptiness = 28;
constexpr idx blank = 26;
const std::string idxstr = "eiaostrnuldgywvpmhfcbzxqkj_    +";
const std::string ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string abc = "abcdefghijklmnopqrstuvwxyz";
const std::array<idx, 32> scores{{1, 1,  1, 1,  1, 1, 1, 1, 1, 1, 2,
                                 2, 4,  4, 4,  3, 3, 4, 4, 3, 3, 10,
                                 8, 10, 5, 8, 0, 0, 0, 0, 0, 0}};
using Rack = std::array<idx, 32>;
}
