#pragma once

namespace archiver::detail {

struct Node {
    Node* left;
    Node* right;
    int freq;
    byte symb;
    Node(int freq) : left(nullptr), right(nullptr), freq(freq) {};
    Node() : left(nullptr), right(nullptr), freq(0) {};
};



}   // namespace detail
