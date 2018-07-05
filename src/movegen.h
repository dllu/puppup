#pragma once
#include <vector>

#include "board.h"
#include "trie.h"
#include "util.h"

namespace puppup {
namespace movegen {

struct Move {
    idx score;
    trie::nodeid node;
    idx cursor;
    idx step;
    idx blanks;
    bool operator<(const Move& other) const { return score < other.score; }
    bool operator>(const Move& other) const { return score > other.score; }
};

namespace __impl {
inline idx turn(idx step) {
    if (step == 1 || step == -1) {
        return -16;
    }
    if (step == 16 || step == -16) {
        return -1;
    }
    return 0;
}

/**
 * generates all moves from a state recursively
 */
void gen(Rack& r, idx step, idx cursor, idx orig_cursor, trie::nodeid node,
         const trie::Gaddag& gaddag, board::State& state,
         idx non_multipliable_score, idx multipliable_score, idx word_mult,
         std::vector<Move>& outputs, idx blanks = 0, idx placed = 0);

idx orthogonal(idx step, idx cursor, const trie::Gaddag& gaddag,
               const board::State& state);

inline bool edge(idx cursor) {
    return cursor % 16 == 15 || cursor < 0 || cursor > 240;
}
}  // __impl

inline void makeMove(board::State& state, const Move& mov,
                     const trie::Gaddag& gaddag) {
    const auto& s = gaddag.getString(mov.node);
    idx cursor = mov.cursor - s.size() * mov.step;

    for (const chr c : s) {
        if (state.board[cursor] == emptiness) {
            state.board[cursor] = c;
            state.letter_score[cursor] = scores[c];
        }
        cursor += mov.step;
    }
    idx blanks = mov.blanks;
    while (blanks) {
        idx blank_cursor = (blanks & idx(0xffffLL)) - 1;
        blanks >>= idx(16LL);
        state.letter_score[blank_cursor] = 0;
    }
    state.score += mov.score;
}

inline std::vector<Move> genFromIdx(Rack r, const trie::Gaddag& gaddag,
                                    board::State state = board::State(),
                                    const idx cursor = board::start) {
    std::vector<Move> outputs;
    __impl::gen(r, -1, cursor, cursor, 0, gaddag, state, 0, 0, 1, outputs);
    return outputs;
}

inline std::vector<Move> genFromBoard(Rack r, const trie::Gaddag& gaddag,
                                      board::State state) {
    std::array<bool, 256> buildable;
    buildable.fill(false);
    for (idx i = 0; i < 240; i++) {
        if (state.board[i] != emptiness) {
            buildable[i - 16] = true;
            buildable[i + 16] = true;
            buildable[i - 1] = true;
            buildable[i + 1] = true;
        }
    }
    for (idx i = 0; i < 240; i++) {
        if (state.board[i] != emptiness || __impl::edge(i)) {
            buildable[i] = false;
        }
    }

    std::vector<Move> outputs;
    constexpr std::array<idx, 2> steps{{1, 16}};
    idx n_buildable = 0;
    for (idx step : steps) {
        for (idx i = 0; i < 240; i++) {
            if (buildable[i]) {
                if (!buildable[i - step]) {
                    __impl::gen(r, -step, i, i, 0, gaddag, state, 0, 0, 1,
                                outputs);
                } else {
                    __impl::gen(r, step, i, i, gaddag.get(0, trie::rev_marker),
                                gaddag, state, 0, 0, 1, outputs);
                }
                n_buildable++;
            }
        }
    }
    if (!n_buildable) {
        return genFromIdx(r, gaddag);
    }
    return outputs;
}

}  // movegen
}
