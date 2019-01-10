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
    explicit Gaddag(std::istream& fin) {
        std::array<idx, 26> stridx;
        for (idx i = 0; i < 26; ++i) {
            stridx[idxstr[i] - 'a'] = i;
        }
        std::string s;
        words_.reserve(200'000);
        while (std::getline(fin, s)) {
            for (char& c : s) {
                if (c >= 'A' && c <= 'Z') {
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
        for (idx w = 0, wn = words_.size(); w < wn; ++w) {
            const auto& word = words_[w];
            const idx n = word.size();
            for (idx start = -1; start < n; ++start) {
                nodeid curr = 0;
                for (idx j = start; j >= 0; --j) {
                    const chr i = word[j];
                    add(curr, i);
                    curr = get(curr, i);
                }
                add(curr, rev_marker);
                curr = get(curr, rev_marker);
                for (idx j = start + 1; j < n; ++j) {
                    const chr i = word[j];
                    add(curr, i);
                    curr = get(curr, i);
                }
                while (curr >= idx(node_to_word_.size())) {
                    node_to_word_.push_back(invalid);
                }
                node_to_word_[curr] = w;
            }
        }
        const nodeid n_nodes = nodes_.size();
        next_sibling_.resize(n_nodes);
        first_child_.resize(n_nodes);
        val_.resize(n_nodes);
        for (nodeid node = 0; node < n_nodes; ++node) {
            for (chr child_chr = 0; child_chr < 32; ++child_chr) {
                nodeid child = get(node, child_chr);
                if (child != invalid) {
                    val_[child] = child_chr;
                }
            }
            for (chr child_chr = 0; child_chr < 32; ++child_chr) {
                nodeid child = get(node, child_chr);
                if (child != invalid) {
                    first_child_[node] = child;
                    break;
                }
            }
            nodeid last = invalid;
            for (chr child_chr = 0; child_chr < 32; ++child_chr) {
                nodeid child = get(node, child_chr);
                if (child != invalid) {
                    if (last != invalid) {
                        next_sibling_[last] = child;
                    }
                    last = child;
                }
            }
            if (last != invalid) {
                next_sibling_[last] = invalid;
            } else {
                first_child_[node] = invalid;
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
    const std::vector<chr>& getStringFromWord(const idx word) const {
        return words_[word];
    }
    const idx nodeToWord(const nodeid curr) const {
        return node_to_word_[curr];
    }
    const bool exists(const nodeid curr) const {
        return node_to_word_[curr] != invalid;
    }
    /*
    const nodeid nextSibling(const nodeid curr) const {
        return next_sibling_[curr];
    }
    const nodeid firstChild(const nodeid curr) const {
        return first_child_[curr];
    }
    const chr val(const nodeid curr) const { return val_[curr]; }
    */

   private:
    std::vector<std::vector<chr>> words_;
    std::vector<Node> nodes_;
    std::vector<idx> node_to_word_;
    std::vector<nodeid> next_sibling_;
    std::vector<nodeid> first_child_;
    std::vector<chr> val_;
};
}  // namespace trie
}  // namespace puppup
