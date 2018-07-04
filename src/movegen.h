#pragma once
#include <vector>

#include "board.h"
#include "trie.h"
#include "util.h"

namespace puppup {
namespace movegen {
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
void gen(Rack& r, idx step, idx cursor, idx orig_cursor, trie::Node* node,
         trie::Node* root, board::State& state, idx non_multipliable_score,
         idx multipliable_score, idx word_mult,
         std::vector<board::State>& outputs);

idx orthogonal(idx step, idx cursor, trie::Node* node,
               const board::State& state);

inline bool edge(idx cursor) {
    return cursor % 16 == 15 || cursor < 0 || cursor > 240;
}
}  // __impl

inline std::vector<board::State> genFromIdx(
    Rack r, const std::unique_ptr<trie::Node>& gaddag,
    board::State state = board::State(), const idx cursor = board::start) {
    std::vector<board::State> outputs;
    __impl::gen(r, -1, cursor, cursor, gaddag.get(), gaddag.get(), state, 0, 0,
                1, outputs);
    return outputs;
}

inline std::vector<board::State> genFromBoard(
    Rack r, const std::unique_ptr<trie::Node>& gaddag, board::State state) {
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

    /*
    board::State zxcv = state;
    for (idx i = 0; i < 240; i++) {
        if (buildable[i]) {
            zxcv.board[i] = gaddag_marker;
        }
    }
    board::print(zxcv);
    */

    std::vector<board::State> outputs;
    constexpr std::array<idx, 2> steps{{1, 16}};
    idx n_buildable = 0;
    for (idx step : steps) {
        for (idx i = 0; i < 240; i++) {
            if (buildable[i]) {
                if (!buildable[i - step]) {
                    __impl::gen(r, -step, i, i, gaddag.get(), gaddag.get(),
                                state, 0, 0, 1, outputs);
                } else {
                    __impl::gen(r, step, i, i, gaddag->at(gaddag_marker),
                                gaddag.get(), state, 0, 0, 1, outputs);
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
