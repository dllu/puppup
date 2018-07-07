#include "movegen.h"
#include <algorithm>
#include <functional>
namespace puppup {
namespace movegen {
namespace __impl {
void gen(Rack& r, idx step, idx cursor, idx orig_cursor, trie::nodeid node,
         const trie::Gaddag& gaddag, board::State& state,
         idx non_multipliable_score, idx multipliable_score, idx word_mult,
         std::vector<Move>& outputs, const bool best_only,
         std::array<idx, 256 * 32>& orthogonal_memo, idx blanks, idx placed) {
    // we have gone far enough in the reverse direction, time to turn around
    if (cursor != orig_cursor && step < 0 &&
        (gaddag.has(node, trie::rev_marker) &&
         (edge(cursor) || state.board[cursor] == emptiness))) {
        gen(r, -step, orig_cursor - step, orig_cursor,
            gaddag.get(node, trie::rev_marker), gaddag, state,
            non_multipliable_score, multipliable_score, word_mult, outputs,
            best_only, orthogonal_memo, blanks, placed);
    }
    // we have found a valid word
    if (!(step == 1 && placed == 1) && step > 0 && gaddag.exists(node) &&
        (edge(cursor) || state.board[cursor] == emptiness)) {
        idx score = non_multipliable_score + multipliable_score * word_mult;
        if (placed >= 7) score += 50;
        if (!best_only || outputs.empty()) {
            outputs.push_back(
                {score, gaddag.nodeToWord(node), cursor, step, blanks});
        } else {
            if (score > outputs.back().score) {
                outputs.back() = {score, gaddag.nodeToWord(node), cursor, step,
                                  blanks};
            }
        }
    }
    // we have gone out of bounds
    if (edge(cursor)) {
        return;
    }
    // traverse over undiscovered land
    if (state.board[cursor] == emptiness) {
        idx word_mult_new = word_mult * board::word_multiplier[cursor];
        /*
        for (trie::nodeid child = gaddag.firstChild(node);
             child != trie::invalid; child = gaddag.nextSibling(child)) {
            chr j = gaddag.val(child);
            if (j >= emptiness) break;
            */
        for (chr j = 0; j < emptiness; j++) {
            if (!gaddag.has(node, j)) continue;
            auto evaluate = [&](const chr i) {
                r[i]--;
                state.board[cursor] = j;
                state.letter_score[cursor] = scores[i];
                idx newblanks = blanks;
                if (i == blank) {
                    newblanks <<= idx(16LL);
                    newblanks |= cursor + 1;
                }
                idx orth_score = orthogonal_memo[cursor * 32 + j];
                if (orth_score == -2) {
                    orth_score = orthogonal(turn(step), cursor, gaddag, state);
                    orthogonal_memo[cursor * 32 + j] = orth_score;
                }
                if (orth_score >= 0) {
                    gen(r, step, cursor + step, orig_cursor,
                        gaddag.get(node, j), gaddag, state,
                        non_multipliable_score + orth_score,
                        multipliable_score +
                            scores[i] * board::letter_multiplier[cursor],
                        word_mult_new, outputs, best_only, orthogonal_memo,
                        newblanks, placed + 1);
                }
                state.board[cursor] = emptiness;
                state.letter_score[cursor] = 0;
                r[i]++;
            };
            if (r[j]) evaluate(j);
            if (r[blank]) evaluate(blank);
        }
    } else {
        // traverse over existing tiles
        const chr i = state.board[cursor];
        if (gaddag.has(node, i)) {
            gen(r, step, cursor + step, orig_cursor, gaddag.get(node, i),
                gaddag, state, non_multipliable_score,
                multipliable_score + state.letter_score[cursor], word_mult,
                outputs, best_only, orthogonal_memo, blanks, placed);
        }
    }
}

idx orthogonal(idx step, idx cursor, const trie::Gaddag& gaddag,
               const board::State& state) {
    if (state.board[cursor + step] == emptiness &&
        state.board[cursor - step] == emptiness) {
        return 0;
    }
    const idx orig_cursor = cursor;
    idx score = state.letter_score[cursor] * board::letter_multiplier[cursor];

    chr node = gaddag.get(0, state.board[cursor]);
    cursor += step;
    idx depth = 0;
    while (!edge(cursor) && state.board[cursor] != emptiness) {
        score += state.letter_score[cursor];
        if (!gaddag.has(node, state.board[cursor])) return -1;
        node = gaddag.get(node, state.board[cursor]);
        cursor += step;
        depth++;
    }
    if (!gaddag.has(node, trie::rev_marker)) return -1;
    node = gaddag.get(node, trie::rev_marker);
    step = -step;
    cursor = orig_cursor + step;
    while (!edge(cursor) && state.board[cursor] != emptiness) {
        score += state.letter_score[cursor];
        if (!gaddag.has(node, state.board[cursor])) return -1;
        node = gaddag.get(node, state.board[cursor]);
        cursor += step;
        depth++;
    }
    if (depth > 0 && !gaddag.exists(node)) return -1;
    score *= board::word_multiplier[orig_cursor];
    if (depth == 0) score = 0;
    return score;
}
}  // __impl
}
}
