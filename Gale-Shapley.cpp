// Gale-Shapley Algorithm

// Gives a stable matching among N men and N women.
// Each member of the proposing party (men in this case) is matched with his best valid partner, while members of the receiving party (women) are matched with their worst valid partners.

// O(N^2) time

#include <iostream>
#include <vector>
#include <numeric>

using namespace std;

// Preference ranges from 0 to N-1, inclusive. Smaller number = higher preference

struct woman {
    vector<int> pList; // a list of preferences, in same order as men. Can also use a map.
    int partner = -1; // current partner, -1 if none
    int rank; // preference of her current partner
};

struct man {
    vector<int> wList; // list of all women, sorted by decreasing preference.
    int next = 0; // next woman in list to propose to according to preference
};

// Pre: size(men) == size(women)
vector<vector<int>> match (vector<man> &men, vector<woman> &women) {
    vector<vector<int>> result;
    vector<int> freeMen(men.size()); // a list of men currently not engaged
    iota(freeMen.begin(), freeMen.end(), 0);
    while (!freeMen.empty()) {
        int cur = freeMen.back();
        man &m = men.at(cur);
        woman &w = women.at(m.wList.at(m.next));
        if (w.partner == -1) {
            w.partner = cur;
            w.rank = w.pList.at(cur);
            freeMen.pop_back();
        } else if (w.pList.at(cur) < w.rank) {
            freeMen.pop_back();
            freeMen.push_back(w.partner);
            w.partner = cur;
            w.rank = w.pList.at(cur);
        }
        m.next++;
    }
    vector<int> temp;
    for (int i=0; i<women.size(); i++) {
        temp.push_back(women.at(i).partner);
        temp.push_back(i);
        result.push_back(temp);
        temp.clear();
    }
    return result;
}
// Post: outputs a stable matching containing man-woman pairs
