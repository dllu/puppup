#pragma once
#include <sstream>
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
         std::vector<Move>& outputs, const bool best_only,
         std::array<idx, 256 * 32 * 2>& orthogonal_memo, idx blanks = 0,
         idx placed = 0);

idx orthogonal(idx step, idx cursor, const trie::Gaddag& gaddag,
               const board::State& state);

inline bool edge(idx cursor) {
    return cursor % 16 == 15 || cursor < 0 || cursor > 240 ||
           board::word_multiplier[cursor] == 0;
}
}  // __impl

inline void makeMove(board::State& state, Rack& rack, Rack& population,
                     const Move& mov, const trie::Gaddag& gaddag) {
    if (mov.word == -1) {
        return;
    }
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
    for (chr c = 0; c < 32; c++) {
        if (rack[c] < 0) {
            population[c] += rack[c];
            rack[c] = 0;
        }
    }
    state.score += mov.score;
}

inline void makeMove(board::State& state, const Move& mov,
                     const trie::Gaddag& gaddag) {
    Rack tmp1 = constArray<32>(999);
    Rack tmp2 = constArray<32>(999);
    makeMove(state, tmp1, tmp2, mov, gaddag);
}

inline Move moveCommand(idx step, idx cursor, std::string word_s,
                        const board::State& state, const trie::Gaddag& gaddag) {
    idx blanks = 0;
    idx non_multipliable_score = 0;
    idx multipliable_score = 0;
    idx word_mult = 1;
    trie::nodeid node = gaddag.get(0, trie::rev_marker);
    idx placed = 0;
    for (char c : word_s) {
        chr letter;
        chr actual_letter;
        if (c >= 'a' && c <= 'z') {
            actual_letter = letter = stridx[(idx)c];
        } else if (c >= 'A' && c <= 'Z') {
            letter = stridx[(idx)c + 'a' - 'A'];
            actual_letter = blank;
            blanks <<= idx(16LL);
            blanks |= cursor + 1;
        } else {
            std::cerr << "unexpected character! " << c << std::endl;
        }
        if (node != -1) {
            node = gaddag.get(node, letter);
        } else {
            std::cerr << "move command contains illegal word! " << word_s
                      << std::endl;
        }
        if (state.board[cursor] == emptiness) {
            placed++;
            multipliable_score +=
                board::letter_multiplier[cursor] * scores[actual_letter];
            word_mult *= board::word_multiplier[cursor];

            idx orth_score = 0;
            bool works = false;
            for (idx o = -1; o <= 1; o += 2) {
                idx ostep = __impl::turn(step) * o;
                for (idx ocursor = cursor + ostep;
                     !__impl::edge(ocursor) &&
                     state.board[ocursor] != emptiness;
                     ocursor += ostep) {
                    orth_score += state.letter_score[ocursor];
                    works = true;
                }
            }

            if (works) {
                orth_score +=
                    board::letter_multiplier[cursor] * scores[actual_letter];
                non_multipliable_score +=
                    orth_score * board::word_multiplier[cursor];
            }
        } else {
            multipliable_score += state.letter_score[cursor];
        }
        cursor += step;
    }
    const idx bingo = placed >= 7 ? 50 : 0;
    const idx score =
        non_multipliable_score + word_mult * multipliable_score + bingo;
    // std::cerr << "nms: " << non_multipliable_score << std::endl;
    // std::cerr << "ms: " << multipliable_score << std::endl;
    // std::cerr << "mult: " << word_mult << std::endl;
    const idx wordid = node == -1 ? -1 : gaddag.nodeToWord(node);
    return Move{score, wordid, cursor, step, blanks};
}

inline std::string toString(const Move& mov, board::State state,
                            const trie::Gaddag& gaddag) {
    std::stringstream ss;
    makeMove(state, mov, gaddag);
    const auto& s = gaddag.getStringFromWord(mov.word);
    idx cursor = mov.cursor - s.size() * mov.step;
    idx x = cursor % 16;
    idx y = cursor / 16;
    if (mov.step == 16) {
        ss << x + 1 << ABC[y] << " ";
    } else {
        ss << ABC[y] << x + 1 << " ";
    }

    for (chr c : s) {
        if (state.letter_score[cursor] == 0) {
            ss << idxSTR[c];
        } else {
            ss << idxstr[c];
        }
        cursor += mov.step;
    }
    return ss.str();
}

inline void print(const Move& mov, board::State state,
                  const trie::Gaddag& gaddag) {
    if (mov.word == -1) {
        std::cout << "CHANGE 7" << std::endl;
    }
    std::cout << toString(mov, state, gaddag) << " " << mov.score << std::endl;
}

inline std::vector<Move> genFromIdx(Rack r, const trie::Gaddag& gaddag,
                                    board::State state = board::State(),
                                    const idx cursor = board::start,
                                    const bool best_only = false) {
    std::vector<Move> outputs;
    auto orthogonal_memo = constArray<256 * 32 * 2>(-2);
    __impl::gen(r, -1, cursor, cursor, 0, gaddag, state, 0, 0, 1, outputs,
                best_only, orthogonal_memo);
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
    idx n_buildable = 0;
    idx last_n = 0;
    for (idx step : {1, 16}) {
        auto orthogonal_memo = constArray<256 * 32 * 2>(-2);
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
                // if (step == 1) std::cerr << "left ";
                // if (step == 16) std::cerr << "down ";
                if (needs_both_directions) {
                    // std::cerr << "generating from " << i / 16 << ", " << i %
                    // 16
                    // << " with both directions" << std::endl;
                    __impl::gen(r, -step, i, i, 0, gaddag, state, 0, 0, 1,
                                outputs, best_only, orthogonal_memo);
                } else {
                    // std::cerr << "generating from " << i / 16 << ", " << i %
                    // 16
                    // << " in forward directions" << std::endl;
                    __impl::gen(r, step, i, i, gaddag.get(0, trie::rev_marker),
                                gaddag, state, 0, 0, 1, outputs, best_only,
                                orthogonal_memo);
                }
                for (idx k = last_n; k < outputs.size(); k++) {
                    // print(outputs[k], state, gaddag);
                }
                last_n = outputs.size();
                n_buildable++;
            }
        }
    }
    if (!n_buildable) {
        return genFromIdx(r, gaddag);
    }
    // std::cerr << "outputs: " << outputs.size() << std::endl;
    return outputs;
}

}  // movegen
}
