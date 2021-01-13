// Implementation of the trie (prefix tree) data structure

#include <vector>
#include <iostream>

using namespace std;

// Modify the following to adapt to a different alphabet
const int SIZE = 26;
const char FIRST = 'a';

struct TrieNode {
    TrieNode* children[SIZE] {};
    int numChildren = 0;
    bool isWord = false;
};

void TrieInsert (TrieNode* root, string word) {
    TrieNode* cur = root;
    for (char c : word) {
        if (cur->children[c-FIRST] == nullptr) {
            cur->children[c-FIRST] = new TrieNode();
            cur->numChildren++;
        }
        cur = cur->children[c-FIRST];
    }
    cur->isWord = true;
}

bool TrieContains (TrieNode* root, string word) {
    TrieNode* cur = root;
    for (char c : word) {
        if (cur->children[c-FIRST] == nullptr) {
            return false;
        }
        cur = cur->children[c-FIRST];
    }
    return cur->isWord;
}

bool TrieHasPrefix (TrieNode* root, string prefix) {
    TrieNode* cur = root;
    for (char c : prefix) {
        if (cur->children[c-FIRST] == nullptr) {
            return false;
        }
        cur = cur->children[c-FIRST];
    }
    return true;
}

void TrieDelete (TrieNode* &root, string word) {
    if (!TrieContains(root, word)) {
        return;
    }
    
    vector<TrieNode*> deQ;
    int front = -1;
    TrieNode* cur = root;
    for (int i=0; i<word.length(); i++) {
        if (cur->numChildren > 1 || cur->isWord) {
            front = i;
        }
        deQ.push_back(cur);
        cur = cur->children[word[i]-FIRST];
    }
    
    if (cur->numChildren > 0) {
        cur->isWord = false;
        return;
    }
    
    for (int i=(int)deQ.size()-1; i>front; i--) {
        delete deQ.back();
        deQ.pop_back();
    }
    if (front == -1) {
        root = nullptr;
    } else {
        deQ.back()->children[word[front]-'a'] = nullptr;
        deQ.back()->numChildren--;
    }
}
