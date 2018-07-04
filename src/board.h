#pragma once
#include <iostream>
#include "util.h"

namespace puppup {
namespace board {
constexpr idx start = 7 + 7 * 16;
// clang-format off
constexpr idx word_multiplier[256] = {
    3, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 3, 0,
    1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 0,
    1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 0,
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0,
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    3, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 3, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 0,
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0,
    1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 0,
    1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 0,
    3, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

constexpr idx letter_multiplier[256] = {
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1,
    2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1,
    1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, 1,
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1,
    1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, 1,
    1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
// clang-format on

struct State {
    idx score;
    std::array<idx, 256> board;
    std::array<idx, 256> letter_score;

    State() : score(0) {
        board.fill(emptiness);
        letter_score.fill(0);
    }
    bool operator<(const State& other) const { return score < other.score; }
    bool operator>(const State& other) const { return score > other.score; }
};

inline void print(const State& state) {
    std::cerr << "Score: " << state.score << std::endl;
    for (idx i = 0; i < 15; i++) {
        std::cerr << "ABCDEFGHIJKLMNOPQRSTUV"[i] << "  ";
        for (idx j = 0; j < 15; j++) {
            if (state.board[i * 16 + j] != emptiness) {
                if (state.letter_score[i * 16 + j] == 0) {
                    std::cerr << "\033[1;33m" << idxstr[state.board[i * 16 + j]]
                              << "\033[0m  ";
                } else {
                    std::cerr << "\033[1;37m" << idxstr[state.board[i * 16 + j]]
                              << "\033[0m  ";
                }
            } else {
                if (letter_multiplier[i * 16 + j] == 2) {
                    std::cerr << "\033[1;36m+\033[0m  ";
                } else if (letter_multiplier[i * 16 + j] == 3) {
                    std::cerr << "\033[1;34m+\033[0m  ";
                } else if (word_multiplier[i * 16 + j] == 2) {
                    std::cerr << "\033[1;35m+\033[0m  ";
                } else if (word_multiplier[i * 16 + j] == 3) {
                    std::cerr << "\033[1;31m+\033[0m  ";
                } else {
                    std::cerr << "   ";
                }
            }
        }
        std::cerr << std::endl;
    }
    std::cerr << "   1  2  3  4  5  6  7  8  9 10 11 12 13 14 15" << std::endl;
}
}
}
