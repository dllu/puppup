#pragma once
#include <array>
#include <locale>
#include <string>

using idx = std::ptrdiff_t;
using chr = idx;
namespace puppup {
constexpr chr emptiness = 27;
constexpr chr blank = 26;
const std::string idxstr = "eiaontrsuldgywvpmhfcbzxqkj_    +";
const std::string idxSTR = "EIAONTRSULDGYWVPMHFCBZXQKJ_    +";
const std::string ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string abc = "abcdefghijklmnopqrstuvwxyz";
using Rack = std::array<idx, 32>;
const Rack scores{{1, 1, 1, 1, 1, 1,  1, 1,  1, 1, 2, 2, 4, 4, 4, 3,
                   3, 4, 4, 3, 3, 10, 8, 10, 5, 8, 0, 0, 0, 0, 0, 0}};
const Rack population{{12, 9, 9, 8, 6, 6, 6, 4, 4, 4, 4, 3, 2, 2, 2, 2,
                       2,  2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0}};
}
