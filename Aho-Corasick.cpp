// The Aho-Corasick Algorithm for multi-pattern matching
// Output sensitive. O(max(input, output)) time complexity

#include <vector>
#include <queue>
#include <iostream>

using namespace std;

// The algorithm currently applies to strings consisting of lower-case English characters but could be easily adapted to a broader set of inputs.
const int SIZE = 26;
const char FIRST = 'a';

// Based on TrieNode
struct ACNode {
    ACNode* children[SIZE] {};
    int pattern = -1; // The index of the pattern in p ending at current node, or -1 if non-existent
    int depth = 0; // 0-indexed
    // Fail is the ending node of the longest prefix in trie which is a proper suffix of the string ending with current node
    // Denotes which node to move to if next match fails
    ACNode* fail = nullptr;
    // Out is the ending node of the longest pattern in trie which is a proper suffix of the string end with current node
    ACNode* out = nullptr;
};

// Builds a trie from given list of patterns, neglecting fail and out pointers
ACNode* buildTrie (vector<string> p) {
    ACNode* root = new ACNode();
    ACNode* cur;
    int charCount;
    string s;
    for (int i=0; i<p.size(); i++) {
        s = p[i];
        cur = root;
        charCount = 0;
        for (char c : s) {
            if (!cur->children[c-FIRST]) {
                cur->children[c-FIRST] = new ACNode();
            }
            cur = cur->children[c-FIRST];
            cur->depth = ++charCount;
        }
        cur->pattern = i;
    }
    return root;
}

// Fills the suffix link (fail pointer) for each node
void fillSuffixLink (ACNode* root) {
    // Traverse the trie in BFS order
    queue<ACNode*> q;
    // The root has no suffix link, while the suffix links of direct descendants of root point to the root
    for (ACNode* child : root->children) {
        if (child) {
            q.push(child);
            child->fail = root;
        }
    }
    ACNode* child;
    ACNode* nextFail;
    while (!q.empty()) {
        // When visiting a node, fills the suffix link for all its children
        for (int i=0; i<SIZE; i++) {
            child = q.front()->children[i];
            if (!child) {
                continue;
            }
            q.push(child);
            nextFail = q.front()->fail;
            // If cur's fail node has a child C with the same value as cur's current child of attention, then
            //  make cur's current child's suffix link point to C.
            // Otherwise recursively search for such a node C in cur's chain of failed pointers until the root is reached,
            //  in which case cur's suffix link must point to root.
            while (nextFail != root && !nextFail->children[i]) {
                nextFail = nextFail->fail;
            }
            if (nextFail != root || (nextFail == root && root->children[i])) {
                child->fail = nextFail->children[i];
            } else {
                child->fail = root;
            }
        }
        q.pop();
    }
}

// Fills the output link for each node. Not all nodes has an output link.
// Pre: all suffix links filled
void fillOutputLink (ACNode* root) {
    // Traverse the trie in BFS order
    queue<ACNode*> q;
    ACNode* cur;
    q.push(root);
    while (!q.empty()) {
        cur = q.front();
        for (ACNode* child : cur->children) {
            if (child) {
                q.push(child);
            }
        }
        // If cur's fail node denotes a pattern, let cur's output link point to it.
        // Otherwise' let cur's output link point to cur's fail node's output node (may be null).
        if (cur != root) {
            if (cur->fail->pattern != -1) {
                cur->out = cur->fail;
            } else {
                cur->out = cur->fail->out;
            }
        }
        q.pop();
    }
}

// Builds an automaton from a list of patterns
ACNode* buildAutomaton (vector<string> p) {
    ACNode* root = buildTrie(p);
    fillSuffixLink(root);
    fillOutputLink(root);
    return root;
}

// Prints all occurrences (start index) of all given patterns in a string
// Pre: p contains distinct patterns
void query (string s, vector<string> p) {
    ACNode* root = buildAutomaton(p);
    ACNode* cur = root;
    ACNode* outl;
    int i = 0;
    while (i < s.length()) {
        if (cur->children[s[i]-'a']) {
            cur = cur->children[s[i]-'a']; // If next character matches, move cur to the matching child node
            outl = cur;
            // If cur denotes the end of a pattern, declare discovery
            if (cur->pattern != -1) {
                cout << "Pattern " << p[cur->pattern] << " found at index " << i - cur->depth + 1 << endl;
            }
            // If the string ending with cur has proper suffixes which are patterns,
            // follow the output link and declare discovery for each pattern found
            while ((outl = outl->out) && (outl->pattern != -1)) {
                cout << "Pattern " << p[outl->pattern] << " found at index " << i - outl->depth + 1 << endl;
            }
            i++;
        } else if (cur != root) {
            cur = cur->fail;
        } else {
            i++;
        }
    }
}
