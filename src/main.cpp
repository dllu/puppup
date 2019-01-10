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
    // std::random_device rd;
    auto gen = std::make_shared<std::mt19937>(/*rd()*/);
    std::ifstream fin(argv[1]);
    auto gaddag = puppup::trie::Gaddag(fin);
    std::cerr << "generated dictionary" << std::endl;
    std::array<idx, 128> stridx;
    stridx.fill(30);
    for (idx i = 0; i < 32; i++) {
        stridx[puppup::idxstr[i]] = i;
    }
    auto coords = genCoords();

    puppup::Rack r{puppup::empty_rack};

    const puppup::board::State state;
    puppup::Game game(state, r, puppup::starting_population, gaddag, gen);
    // std::ifstream test_fin("../test.txt");
    std::string s;
    while (std::getline(std::cin, s)) {
        auto go = [&]() {
            if (std::accumulate(r.begin(), r.end(), idx(0)) == 0) {
                std::cerr << "No tiles in rack!" << std::endl;
                return;
            }
            auto tic = std::chrono::steady_clock::now();

            puppup::movegen::Move best = game.thinkyThinky();

            auto toc = std::chrono::steady_clock::now();
            // auto tac = std::chrono::steady_clock::now();

            {
                auto state2 = game.state_;
                makeMove(state2, best, gaddag);
                puppup::board::print(state2, game.state_);
            }

            std::chrono::duration<double> elapsed = toc - tic;
            // std::chrono::duration<double> elapsed2 = tac - toc;
            std::cerr << "Best computed in " << elapsed.count() << " seconds"
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
            game.setRack(game.turn_, r);
            game.setRack(game.turn_ ^ 1, puppup::empty_rack);
            go();
        } else if (coords.count(command)) {
            auto step_cur = coords[command];
            ss >> argument;
            idx cur = step_cur.second;
            idx step = step_cur.first;
            puppup::movegen::Move mov = puppup::movegen::moveCommand(
                step, cur, argument, game.state_, gaddag);
            puppup::movegen::print(mov, game.state_, gaddag);
            game.ply(mov, false);
            puppup::board::print(game.state_);
        } else if (command == "go") {
            go();
        } else if (command == "pass") {
            game.turn_ ^= 1;
            game.state_.score *= -1;
            puppup::board::print(game.state_);
        }
        // }
        // std::this_thread::sleep_for(std::chrono::milliseconds(17));
    }
    return 0;
}
