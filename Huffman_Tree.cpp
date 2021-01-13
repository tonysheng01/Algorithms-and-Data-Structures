// HUFFMAN TREE AND HUFFMAN ENCODING ------------------------------------------------------------------------

// Huffman Tree: Given a list of weighted items, arranges them in a binary tree that minimizes the sum of
//  (depth(i) * weight(i)) through all items i in the list.
// Huffman Encoding: Given a list of characters and their frequencies, finds a set of prefix codes consisting
//  of 0 and 1 that minimizes the sum of (lengthofCode(c) * frequency(c)) through all characters c.


#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>

using namespace std;

struct HTNode {
    char name;
    double weight;
    HTNode* left;
    HTNode* right;
};

struct cmp {
    bool operator()(HTNode* a, HTNode* b) {
        return a->weight > b->weight;
    }
};

// Builds and returns the root of a Huffman tree with given a map of character weights
HTNode* buildHT(unordered_map<char,double> weights) {
    priority_queue<HTNode*, vector<HTNode*>, cmp> pq;
    for (auto &i : weights) {
        pq.push(new HTNode{i.first, i.second, nullptr, nullptr});
    }
    HTNode *first, *second;
    // In each iteration, take the two nodes with minimum weight in the pq and subject them to a newly-created
    //  parent with weight as their sum
    while (pq.size() > 1) {
        first = pq.top();
        pq.pop();
        second = pq.top();
        pq.pop();
        // Non-leaf nodes are assigned name '\0'
        pq.push(new HTNode{0, first->weight+second->weight, first, second});
    }
    return pq.top();
}

// Builds a Huffman tree and returns therefrom a map of prefix codes given a map of character weights
unordered_map<char,string> getHuffmanCode(unordered_map<char,double> weights) {
    unordered_map<char,string> encoding;
    // Builds Huffman tree
    HTNode* root = buildHT(weights);
    
    // If there is only one character, encode it with "0"
    if (!root->left) {
        encoding[root->name] = "0";
        return encoding;
    }
    
    HTNode* cur = root;
    stack<pair<HTNode*,bool>> s;
    string code;
    
    // Uses a post-order-like traversal scheme. However, codes are assigned in a pre-order fashion.
    while (true) {
        if (cur) {
            if (s.empty() || cur != s.top().first) {
                s.push(make_pair(cur, false));
                if (cur->name != 0) {
                    encoding[cur->name] = code; // Only nodes that represent characters are encoded
                }
                if (cur = cur->left) {
                    code.push_back('0');
                }
            } else if (!s.top().second) {
                s.top().second = true;
                if (cur = cur->right) {
                    code.push_back('1');
                }
            } else {
                s.pop();
                code.pop_back();
                if (s.empty()) {
                    break;
                } else {
                    cur = s.top().first;
                }
            }
        } else if (s.empty()) {
            break;
        } else {
            cur = s.top().first;
        }
    }
    
    return encoding;
}

// ----------------------------------------------------------------------------------------------------------
