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

inline void print(const State& state, const State& other) {
    std::cout << "Score: " << state.score << std::endl;
    for (idx i = 0; i < 15; i++) {
        std::cout << "ABCDEFGHIJKLMNOPQRSTUV"[i] << " ";
        for (idx j = 0; j < 15; j++) {
            if (state.board[i * 16 + j] != emptiness) {
                if (state.board[i * 16 + j] == other.board[i * 16 + j]) {
                    if (state.letter_score[i * 16 + j] == 0) {
                        std::cout << "\033[1;103;31m "
                                  << idxSTR[state.board[i * 16 + j]]
                                  << " \033[0m";
                    } else {
                        std::cout << "\033[1;103;30m "
                                  << idxSTR[state.board[i * 16 + j]]
                                  << " \033[0m";
                    }
                } else {
                    if (state.letter_score[i * 16 + j] == 0) {
                        std::cout << "\033[1;33;31m "
                                  << idxSTR[state.board[i * 16 + j]]
                                  << " \033[0m";
                    } else {
                        std::cout << "\033[1;37;40m "
                                  << idxSTR[state.board[i * 16 + j]]
                                  << " \033[0m";
                    }
                }
            } else {
                if (letter_multiplier[i * 16 + j] == 2) {
                    std::cout << "\033[1;46m + \033[0m";
                } else if (letter_multiplier[i * 16 + j] == 3) {
                    std::cout << "\033[1;104m + \033[0m";
                } else if (word_multiplier[i * 16 + j] == 2) {
                    std::cout << "\033[1;45m + \033[0m";
                } else if (word_multiplier[i * 16 + j] == 3) {
                    std::cout << "\033[1;41m + \033[0m";
                } else {
                    std::cout << "\033[47m   \33[0m";
                }
            }
        }
        std::cout << "\n";
    }
    std::cout << "   1  2  3  4  5  6  7  8  9 10 11 12 13 14 15" << std::endl;
}

inline void print(const State& state) { print(state, state); }
}
}
