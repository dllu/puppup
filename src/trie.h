#pragma once
#include <array>
#include <fstream>
#include <memory>
#include <string>

#include "util.h"

namespace puppup {
namespace trie {

using nodeid = idx;
constexpr idx invalid = -1;
constexpr chr rev_marker = 31;

class Gaddag {
    struct Node {
        std::array<idx, 32> children;
        Node() { children.fill(invalid); }
    };

   public:
    Gaddag(std::istream& fin) {
        std::array<idx, 26> stridx;
        for (idx i = 0; i < 26; i++) {
            stridx[idxstr[i] - 'a'] = i;
        }
        std::string s;
        words_.reserve(200'000);
        while (std::getline(fin, s)) {
            for (char& c : s) {
                if (c >= 'A' || c <= 'Z') {
                    c += 'a' - 'A';
                }
                if (c < 'a' || c > 'z') {
                    std::cerr << s << std::endl;
                    break;
                }
            }
            words_.emplace_back();
            words_.back().reserve(s.length());
            for (char c : s) {
                words_.back().push_back(stridx[c - 'a']);
            }
        }
        nodes_.reserve(5 * words_.size());
        nodes_.emplace_back();  // root
        node_to_word_.reserve(5 * words_.size());
        for (idx w = 0, wn = words_.size(); w < wn; w++) {
            const auto& word = words_[w];
            const idx n = word.size();
            for (idx start = -1; start < n; start++) {
                nodeid curr = 0;
                for (idx j = start; j >= 0; j--) {
                    const chr i = word[j];
                    add(curr, i);
                    curr = get(curr, i);
                }
                add(curr, rev_marker);
                curr = get(curr, rev_marker);
                for (idx j = start + 1; j < n; j++) {
                    const chr i = word[j];
                    add(curr, i);
                    curr = get(curr, i);
                }
                while (curr >= node_to_word_.size()) {
                    node_to_word_.push_back(invalid);
                }
                node_to_word_[curr] = w;
            }
        }
    }

    void add(const nodeid curr, const chr child) {
        if (!has(curr, child)) {
            nodes_[curr].children[child] = nodes_.size();
            nodes_.emplace_back();
        }
    }

    const nodeid get(const nodeid curr, const chr child) const {
        return nodes_[curr].children[child];
    }
    const bool has(const nodeid curr, const chr child) const {
        return get(curr, child) != invalid;
    }
    const std::vector<chr>& getString(const nodeid curr) const {
        return words_[node_to_word_[curr]];
    }
    const bool exists(const nodeid curr) const {
        return node_to_word_[curr] != invalid;
    }

   private:
    std::vector<std::vector<chr>> words_;
    std::vector<Node> nodes_;
    std::vector<idx> node_to_word_;
};
}
}
