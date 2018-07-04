#include "movegen.h"
#include <algorithm>
#include <functional>
namespace puppup {
namespace movegen {
namespace __impl {
void gen(Rack& r, idx step, idx cursor, idx orig_cursor, trie::Node* node,
         trie::Node* root, board::State& state, idx non_multipliable_score,
         idx multipliable_score, idx word_mult,
         std::vector<board::State>& outputs) {
    // we have gone far enough in the reverse direction, time to turn around
    if (cursor != orig_cursor && step < 0 &&
        (node->children[gaddag_marker] &&
         (edge(cursor) || state.board[cursor] == emptiness))) {
        gen(r, -step, orig_cursor - step, orig_cursor, node->at(gaddag_marker),
            root, state, non_multipliable_score, multipliable_score, word_mult,
            outputs);
    }
    // we have found a valid word
    if (step > 0 && node->exists &&
        (edge(cursor) || state.board[cursor] == emptiness)) {
        outputs.push_back(state);
        outputs.back().score +=
            non_multipliable_score + multipliable_score * word_mult;
        if (std::accumulate(r.begin(), r.end(), idx(0)) == 0) {
            outputs.back().score += 50;
        }
    }
    // we have gone out of bounds
    if (edge(cursor)) {
        return;
    }
    // traverse over undiscovered land
    if (state.board[cursor] == emptiness) {
        idx word_mult_new = word_mult * board::word_multiplier[cursor];
        for (idx j = 0; j < emptiness; j++) {
            if (!node->children[j]) continue;
            auto evaluate = [&](idx i) {
                r[i]--;
                state.board[cursor] = j;
                state.letter_score[cursor] = scores[i];
                idx orth_score = orthogonal(turn(step), cursor, root, state);
                if (orth_score >= 0) {
                    gen(r, step, cursor + step, orig_cursor, node->at(j), root,
                        state, non_multipliable_score + orth_score,
                        multipliable_score +
                            scores[i] * board::letter_multiplier[cursor],
                        word_mult_new, outputs);
                }
                // backtrack
                state.board[cursor] = emptiness;
                state.letter_score[cursor] = 0;
                r[i]++;
            };
            if (r[j]) evaluate(j);
            if (r[blank]) evaluate(blank);
        }
    } else {
        // traverse over existing tiles
        const idx i = state.board[cursor];
        if (node->children[i]) {
            gen(r, step, cursor + step, orig_cursor, node->at(i), root, state,
                non_multipliable_score,
                multipliable_score + state.letter_score[cursor], word_mult,
                outputs);
        }
    }
}

idx orthogonal(idx step, idx cursor, trie::Node* node,
               const board::State& state) {
    const idx orig_cursor = cursor;
    idx score = state.letter_score[cursor] * board::letter_multiplier[cursor];
    node = node->at(state.board[cursor]);
    cursor += step;
    idx depth = 0;
    while (!edge(cursor) && state.board[cursor] != emptiness) {
        score += state.letter_score[cursor];
        if (!node->children[state.board[cursor]]) return -1;
        node = node->at(state.board[cursor]);
        cursor += step;
        depth++;
    }
    if (!node->children[gaddag_marker]) return -1;
    node = node->at(gaddag_marker);
    step = -step;
    cursor = orig_cursor + step;
    while (!edge(cursor) && state.board[cursor] != emptiness) {
        score += state.letter_score[cursor];
        if (!node->children[state.board[cursor]]) return -1;
        node = node->at(state.board[cursor]);
        cursor += step;
        depth++;
    }
    if (depth > 0 && !node->exists) return -1;
    score *= board::word_multiplier[orig_cursor];
    if (depth == 0) score = 0;
    return score;
}
}  // __impl
}
}
