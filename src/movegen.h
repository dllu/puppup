#pragma once
#include <tuple>
#include <vector>

#include "board.h"
#include "trie.h"
#include "util.h"

namespace puppup {
namespace movegen {

struct Move {
    idx score;
    idx word;
    idx cursor;
    idx step;
    idx blanks;
    bool operator<(const Move& other) const { return score < other.score; }
    bool operator>(const Move& other) const {
        return score > other.score ||
               (score == other.score &&
                (word > other.word ||
                 (word == other.word &&
                  (cursor > other.cursor ||
                   (cursor == other.cursor &&
                    (step > other.step ||
                     (step == other.step && (blanks > other.blanks))))))));
    }
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
         std::vector<Move>& outputs, const bool best_only, idx blanks = 0,
         idx placed = 0);

idx orthogonal(idx step, idx cursor, const trie::Gaddag& gaddag,
               const board::State& state);

inline bool edge(idx cursor) {
    return cursor % 16 == 15 || cursor < 0 || cursor > 240;
}
}  // __impl

inline void makeMove(board::State& state, Rack& rack, const Move& mov,
                     const trie::Gaddag& gaddag) {
    const auto& s = gaddag.getStringFromWord(mov.word);
    idx cursor = mov.cursor - s.size() * mov.step;

    for (const chr c : s) {
        if (state.board[cursor] == emptiness) {
            state.board[cursor] = c;
            state.letter_score[cursor] = scores[c];
            rack[c]--;
        }
        cursor += mov.step;
    }
    idx blanks = mov.blanks;
    while (blanks) {
        idx blank_cursor = (blanks & idx(0xffffLL)) - 1;
        blanks >>= idx(16LL);
        state.letter_score[blank_cursor] = 0;
        rack[blank]--;
        rack[state.board[blank_cursor]]++;
    }
    state.score += mov.score;
}

inline void print(const Move& mov, board::State state,
                  const trie::Gaddag& gaddag) {
    Rack tmp;
    makeMove(state, tmp, mov, gaddag);
    const auto& s = gaddag.getStringFromWord(mov.word);
    idx cursor = mov.cursor - s.size() * mov.step;
    idx x = cursor % 16;
    idx y = cursor / 16;
    if (mov.step == 16) {
        std::cout << x + 1 << ABC[y] << " ";
    } else {
        std::cout << ABC[y] << x + 1 << " ";
    }

    for (chr c : s) {
        if (state.letter_score[cursor] == 0) {
            std::cout << idxSTR[c];
        } else {
            std::cout << idxstr[c];
        }
        cursor += mov.step;
    }

    std::cout << " " << mov.score << std::endl;
}

inline std::vector<Move> genFromIdx(Rack r, const trie::Gaddag& gaddag,
                                    board::State state = board::State(),
                                    const idx cursor = board::start,
                                    const bool best_only = false) {
    std::vector<Move> outputs;
    __impl::gen(r, -1, cursor, cursor, 0, gaddag, state, 0, 0, 1, outputs,
                best_only);
    return outputs;
}

inline std::vector<Move> genFromBoard(Rack r, const trie::Gaddag& gaddag,
                                      board::State state,
                                      const bool best_only = false) {
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
                bool needs_both_directions = false;
                // todo: faster method than this ridiculous O(n^3) one
                if (!buildable[i - step]) {
                    needs_both_directions = true;
                    idx j = i - step;
                    while (!__impl::edge(j) && state.board[j] != emptiness) {
                        if (buildable[j]) {
                            needs_both_directions = false;
                            break;
                        }
                        j -= step;
                    }
                }
                if (needs_both_directions) {
                    __impl::gen(r, -step, i, i, 0, gaddag, state, 0, 0, 1,
                                outputs, best_only);
                } else {
                    __impl::gen(r, step, i, i, gaddag.get(0, trie::rev_marker),
                                gaddag, state, 0, 0, 1, outputs, best_only);
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
