#pragma once
#include <numeric>
#include <random>
#include <set>
#include "board.h"
#include "movegen.h"
#include "util.h"

namespace puppup {
class Game {
   public:
    Game(const board::State& state, const Rack& rack, const Rack& population,
         const trie::Gaddag& gaddag, std::shared_ptr<std::mt19937> gen)
        : gen_(gen),
          turn_(0),
          state_{state},
          racks_{{rack, empty_rack}},
          population_(population),
          out_of_tiles_(false),
          winner_(-1),
          gaddag_(gaddag) {
        for (idx i = 0; i < 240; i++) {
            if (state.board[i] != emptiness) {
                population_[state.board[i]]--;
            }
        }
        for (chr i = 0; i < 32; i++) {
            population_[i] -= rack[i];
        }
    }
    void genRack(idx r) { fillRack(racks_[r]); }

    void ply(movegen::Move m) {
        if (winner_ != -1) {
            return;
        }
        movegen::makeMove(state_, racks_[turn_], m, gaddag_);
        if (!out_of_tiles_) {
            fillRack(racks_[turn_]);
        } else if (!std::accumulate(racks_[turn_].begin(), racks_[turn_].end(),
                                    idx(0))) {
            checkWinner();
            return;
        }
        turn_ ^= 1;
        state_.score *= -1;
    }

    idx rollout(idx numply) {
        idx blocked = 0;
        while (winner_ == -1 && numply--) {
            auto moves =
                movegen::genFromBoard(racks_[turn_], gaddag_, state_, true);
            if (!moves.empty()) {
                blocked = 0;
                /*
                const idx n = moves.size();
                std::vector<idx> move_scores(n);
                for (idx i = 0; i < n; i++) {
                    move_scores[i] = moves[i].score;
                }
                std::discrete_distribution<idx> distribution(
                    move_scores.begin(), move_scores.end());
                idx drawn = distribution(*gen_);
                    */
                /*
                std::sort(moves.begin(), moves.end(),
                          std::greater<puppup::movegen::Move>());
                          */
                idx drawn = 0;
                // movegen::print(moves[drawn], state_, gaddag_);
                ply(moves[drawn]);
            } else {
                blocked++;
                turn_ ^= 1;
                state_.score *= -1;
            }
            // adjudicate the game if both players can't play
            if (blocked >= 2 || (winner_ == -1 && !numply)) {
                checkWinner();
                break;
            }
        }
        // board::print(state_);
        return winner_;
    }

    idx winProbability(idx samples, const movegen::Move& mov, idx max_losses) {
        ply(mov);
        idx wins = 0;
        idx trial = 0;
        for (; trial < samples; trial++) {
            Game other(*this);
            other.genRack(1);
            idx winner = other.rollout(4);
            if (winner == 0) {
                wins++;
            }
            if (trial + 1 - wins >= max_losses) {
                trial++;
                break;
            }
        }
        std::cerr << "Random sample results: " << wins << " / " << trial
                  << std::endl;
        return wins;
    }

    movegen::Move thinkyThinky(idx max_moves = 10, idx samples = 300) {
        auto moves = movegen::genFromBoard(racks_[turn_], gaddag_, state_);
        std::set<movegen::Move, std::greater<movegen::Move>> moves_set(
            moves.begin(), moves.end());
        movegen::Move best = moves[0];
        idx best_prob = 0;
        for (auto& mov : moves_set) {
            movegen::print(mov, state_, gaddag_);
            Game other(*this);
            const idx prob =
                other.winProbability(samples, mov, samples - best_prob);
            if (prob > best_prob) {
                best = mov;
                best_prob = prob;
            }

            max_moves--;
            if (max_moves == 0) break;
        }
        return best;
    }

   private:
    void checkWinner() {
        for (chr i = 0; i < 27; i++) {
            state_.score += scores[i] * racks_[turn_ ^ 1][i];
        }
        if (state_.score > 0) {
            winner_ = turn_;
        } else if (state_.score < 0) {
            winner_ = turn_ ^ 1;
        } else {
            winner_ = 2;
        }
    }
    template <class T>
    void fillRack(T& container) {
        const idx need =
            7 - std::accumulate(container.begin(), container.end(), idx(0));
        for (idx adds = 0; adds < need; adds++) {
            std::discrete_distribution<idx> distribution(population_.begin(),
                                                         population_.end());
            idx drawn = distribution(*gen_);
            container[drawn]++;
            population_[drawn]--;

            if (std::accumulate(population_.begin(), population_.end(),
                                idx(0)) == 0) {
                out_of_tiles_ = true;
                break;
            }
        }
    }
    std::shared_ptr<std::mt19937> gen_;
    idx turn_;
    board::State state_;
    std::array<Rack, 2> racks_;
    Rack population_;
    bool out_of_tiles_;
    idx winner_;
    const trie::Gaddag& gaddag_;
};
}
