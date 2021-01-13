// KMP Algorithm for linear-time pattern searching

#include <vector>
#include <iostream>

using namespace std;

// Longest proper prefix which is also a suffix
vector<int> findLPS (string p) {
    vector<int> lps {0};
    int i = 1;
    int len = 0;
    
    while (i < p.length()) {
        if (p[i] == p[len]) {
            lps.push_back(++len);
            i++;
        } else if (len > 0) {
            len = lps[len-1];
        } else {
            lps.push_back(0);
            i++;
        }
    }
    
    return lps;
}

// Finds the index of first occurence of p in s, or -1 if not found
int findFirst (string s, string p) {
    vector<int> lps = findLPS(p);
    int i = 0;
    int j = 0;
    
    while (i + p.length() - j <= s.length()) {
        if (j == p.length()) {
            return i - j;
        }
        
        if (s[i] == p[j]) {
            i++;
            j++;
        } else if (j == 0) {
            i++;
        } else {
            j = lps[j-1];
        }
    }
    
    return -1;
}

// Finds all occurrences of p in s
vector<int> findAll (string s, string p) {
    vector<int> indexes;
    vector<int> lps = findLPS(p);
    int i = 0;
    int j = 0;
    
    while (i + p.length() - j <= s.length()) {
        if (j == p.length()) {
            indexes.push_back(i-j);
            j = lps[j-1];
        }
        
        if (s[i] == p[j]) {
            i++;
            j++;
        } else if (j == 0) {
            i++;
        } else {
            j = lps[j-1];
        }
    }
    
    return indexes;
}
