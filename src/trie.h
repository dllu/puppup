#pragma once
#include <array>
#include <fstream>
#include <memory>
#include <string>

#include "util.h"

namespace puppup {
namespace trie {

struct Node {
    std::array<std::unique_ptr<Node>, 32> children;
    bool exists = false;

    void add(const idx i) {
        if (!children[i]) {
            children[i] = std::make_unique<Node>();
        }
    }
    Node* at(const idx i) { return children[i].get(); }
};

inline std::unique_ptr<Node> dictionary(std::ifstream& fin) {
    std::unique_ptr<Node> root = std::make_unique<Node>();
    std::string s;
    std::array<idx, 26> stridx;
    for (idx i = 0; i < 26; i++) {
        stridx[idxstr[i] - 'a'] = i;
    }
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
        const idx n = s.length();
        for (idx start = -1; start < n; start++) {
            Node* curr = root.get();
            for (idx j = start; j >= 0; j--) {
                const idx i = stridx[s[j] - 'a'];
                curr->add(i);
                curr = curr->at(i);
            }
            curr->add(gaddag_marker);
            curr = curr->at(gaddag_marker);
            for (idx j = start + 1; j < n; j++) {
                const idx i = stridx[s[j] - 'a'];
                curr->add(i);
                curr = curr->at(i);
            }
            curr->exists = true;
        }
    }
    return root;
}

inline void print(const std::unique_ptr<Node>& curr, std::string s = "") {
    if (curr->exists) {
        std::cerr << s << std::endl;
    }
    for (idx i = 0; i < 32; i++) {
        if (curr->children[i]) {
            print(curr->children[i], s + idxstr[i]);
        }
    }
}
}
}
