#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <sstream>
#include <thread>
#include <unordered_map>

#include "board.h"
#include "game.h"
#include "movegen.h"
#include "trie.h"

std::unordered_map<std::string, std::pair<idx, idx>> genCoords() {
    std::unordered_map<std::string, std::pair<idx, idx>> coords;
    for (idx i = 0; i < 256; i++) {
        idx x = i % 16;
        idx y = i / 16;
        {
            std::stringstream ss;
            ss << x + 1 << puppup::abc[y];
            coords[ss.str()] = std::make_pair((idx)16, i);
        }
        {
            std::stringstream ss;
            ss << puppup::abc[y] << x + 1;
            coords[ss.str()] = std::make_pair((idx)1, i);
        }
        {
            std::stringstream ss;
            ss << x + 1 << puppup::ABC[y];
            coords[ss.str()] = std::make_pair((idx)16, i);
        }
        {
            std::stringstream ss;
            ss << puppup::ABC[y] << x + 1;
            coords[ss.str()] = std::make_pair((idx)1, i);
        }
    }
    return coords;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "USAGE: ./puppup /path/to/dictionary" << std::endl;
    }
    std::random_device rd;
    auto gen = std::make_shared<std::mt19937>(rd());
    std::ifstream fin(argv[1]);
    auto gaddag = puppup::trie::Gaddag(fin);
    std::cerr << "generated dictionary" << std::endl;
    std::array<idx, 128> stridx;
    stridx.fill(30);
    for (idx i = 0; i < 32; i++) {
        stridx[puppup::idxstr[i]] = i;
    }
    auto coords = genCoords();

    // while (true) {
    puppup::Rack r{puppup::empty_rack};
    puppup::board::State state;

    /*
    // for (char c : std::string("quetzal")) {
    for (char c : std::string("vuvucww")) {
        r[stridx[c]]++;
    }

    puppup::Game game(state, r, puppup::starting_population, gaddag, gen);
    game.winProbability(500);
    return 0;
    */
    // std::ifstream test_fin("../test.txt");
    std::string s;
    while (std::getline(std::cin, s)) {
        auto go = [&]() {
            if (std::accumulate(r.begin(), r.end(), idx(0)) == 0) {
                std::cerr << "No tiles in rack!" << std::endl;
                return;
            }
            state.score = 0;
            auto tic = std::chrono::steady_clock::now();
            auto moves = puppup::movegen::genFromBoard(r, gaddag, state);
            std::sort(moves.begin(), moves.end(),
                      std::greater<puppup::movegen::Move>());

            auto toc = std::chrono::steady_clock::now();
            puppup::Game game(state, r, puppup::starting_population, gaddag,
                              gen);
            puppup::movegen::Move best = game.thinkyThinky();

            auto tac = std::chrono::steady_clock::now();

            {
                auto state2 = state;
                puppup::Rack tmp{puppup::empty_rack};
                makeMove(state2, tmp, best, gaddag);
                puppup::board::print(state2, state);
            }

            std::chrono::duration<double> elapsed = toc - tic;
            std::cerr << "Moves generated: " << moves.size() << " in "
                      << elapsed.count() << " seconds" << std::endl;
            std::chrono::duration<double> elapsed2 = tac - toc;
            std::cerr << "Best computed in " << elapsed2.count() << " seconds"
                      << std::endl;
        };
        std::stringstream ss(s);
        std::string command, argument;
        ss >> command;
        if (command == "rack") {
            r.fill(0);
            ss >> argument;
            for (char c : argument) {
                r[stridx[c]]++;
            }
            go();
        } else if (coords.count(command)) {
            auto step_cur = coords[command];
            ss >> argument;
            idx cur = step_cur.second;
            idx step = step_cur.first;
            for (char c : argument) {
                if (c >= 'a' && c <= 'z') {
                    idx ch = stridx[c];
                    state.board[cur] = ch;
                    state.letter_score[cur] = puppup::scores[ch];
                } else if (c >= 'A' && c <= 'Z') {
                    idx ch = stridx[c - 'A' + 'a'];
                    state.board[cur] = ch;
                    state.letter_score[cur] = 0;
                }
                cur += step;
            }
        } else if (command == "go") {
            go();
        }
        // }
        // std::this_thread::sleep_for(std::chrono::milliseconds(17));
    }
    return 0;
}
