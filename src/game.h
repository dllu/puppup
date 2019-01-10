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
        for (idx i = 0; i < 240; ++i) {
            if (state.board[i] != emptiness) {
                if (state.letter_score[i] == 0) {
                    --population_[blank];
                } else {
                    --population_[state.board[i]];
                }
            }
        }
        for (chr i = 0; i < 32; ++i) {
            population_[i] -= rack[i];
        }
    }
    void setRack(idx turn, Rack r) {
        for (chr i = 0; i < 32; i++) {
            population_[i] += racks_[turn][i] - r[i];
            racks_[turn][i] = r[i];
        }
    }
    void genRack(idx r) {
        if (std::accumulate(population_.begin(), population_.end(), idx(0)) ==
            0) {
            out_of_tiles_ = true;
            return;
        }
        fillRack(racks_[r]);
    }

    void ply(movegen::Move m, const bool fill_rack = true) {
        if (winner_ != -1) {
            return;
        }
        movegen::makeMove(state_, racks_[turn_], population_, m, gaddag_);
        if (fill_rack) {
            if (!out_of_tiles_) {
                fillRack(racks_[turn_]);
            } else if (!std::accumulate(racks_[turn_].begin(),
                                        racks_[turn_].end(), idx(0))) {
                checkWinner();
                return;
            }
        }
        turn_ ^= 1;
        state_.score *= -1;
    }

    idx rollout(idx numply) {
        idx blocked = 0;
        while (winner_ == -1 && numply--) {
            auto moves =
                movegen::genFromBoard(racks_[turn_], gaddag_, state_, false);
            if (!moves.empty()) {
                blocked = 0;
                const idx n = moves.size();
                std::vector<idx> move_scores(n);
                for (idx i = 0; i < n; ++i) {
                    move_scores[i] = moves[i].score * moves[i].score;
                }
                std::discrete_distribution<idx> distribution(
                    move_scores.begin(), move_scores.end());
                idx drawn = distribution(*gen_);
                /*
                std::sort(moves.begin(), moves.end(),
                          std::greater<puppup::movegen::Move>());
                idx drawn = 0;
                */

                // movegen::print(moves[drawn], state_, gaddag_);
                ply(moves[drawn]);
            } else {
                ++blocked;
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

    idx score() const {
        if (turn_ == 0)
            return state_.score;
        else
            return -state_.score;
    }

    idx winProbability(const idx samples, const movegen::Move& mov,
                       const idx maxfails) {
        idx wins = 0;
        idx trial = 0;

        idx sumscore = 0;
        for (; trial < samples; ++trial) {
            if (trial - wins >= maxfails) {
                break;
            }
            Game other(*this);
            other.setRack(turn_ ^ 1, empty_rack);
            other.genRack(turn_ ^ 1);
            other.ply(mov);
            // print(other.racks_[0]);
            // print(other.racks_[1]);
            idx winner = other.rollout(99);
            if (winner == 0) {
                ++wins;
            }
            // std::cerr << "trial score: " << other.score() << std::endl;
            sumscore += other.score();
        }
        std::cerr << "Random sample results: " << wins << " / " << trial << "; "
                  << sumscore << std::endl;
        return wins;
    }

    movegen::Move thinkyThinky(idx max_moves = 5, const idx samples = 50) {
        auto moves = movegen::genFromBoard(racks_[turn_], gaddag_, state_);
        std::set<movegen::Move, std::greater<movegen::Move>> moves_set(
            moves.begin(), moves.end());
        movegen::Move best = *moves_set.begin();
        //auto best_prob = -9999999999LL;
        //idx maxfails = samples;
        for (auto& mov : moves_set) {
            movegen::print(mov, state_, gaddag_);
            /*
            Game other(*this);
            const auto prob = other.winProbability(samples, mov, maxfails);
            if (prob > best_prob) {
                best = mov;
                best_prob = prob;
                maxfails = samples - prob;
            }
            */

            --max_moves;
            if (max_moves == 0) break;
        }
        /*
        {
            Game other(*this);
            setRack(turn_, empty_rack);
            genRack(turn_);
            movegen::Move change = {0, -1, 0, 0, 0};
            movegen::print(change, state_, gaddag_);
            const auto prob = other.winProbability(samples, change, maxfails);
            if (prob > best_prob) {
                return change;
            }
        }
        */
        return best;
    }

    // private:
   public:
    void checkWinner() {
        for (chr i = 0; i < 27; ++i) {
            state_.score += scores[i] * racks_[turn_ ^ 1][i];
        }
        // print(racks_[turn_]);
        // print(racks_[turn_ ^ 1]);
        // std::cerr << state_.score << std::endl;
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
        if (out_of_tiles_) {
            return;
        }
        const idx need =
            7 - std::accumulate(container.begin(), container.end(), idx(0));
        // std::cerr << "container before: ";
        // print(container);
        // std::cerr << "population before: ";
        // print(population_);
        for (idx adds = 0; adds < need; ++adds) {
            std::discrete_distribution<idx> distribution(population_.begin(),
                                                         population_.end());
            idx drawn = distribution(*gen_);
            ++container[drawn];
            --population_[drawn];

            if (std::accumulate(population_.begin(), population_.end(),
                                idx(0)) == 0) {
                out_of_tiles_ = true;
                break;
            }
        }
        // std::cerr << "container after: ";
        // print(container);
        // std::cerr << "population after: ";
        // print(population_);
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
}  // namespace puppup
