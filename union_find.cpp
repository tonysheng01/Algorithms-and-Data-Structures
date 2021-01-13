// Union-Find Data Structure and its Applications

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

typedef pair<int,int> pi;
typedef vector<vector<int>> tree;

// UNION FIND -----------------------------------------------------------------------------------------------

struct UFNode {
    int val;
    UFNode* next;
};

// Requires input to be distinct ints from 0 to n-1; otherwise vectors need to be replaced with maps
struct UnionFind {
    vector<UFNode*> nodes;
    vector<int> groupSize;
};

// Returns a union-find given a vector of distinct ints from 0 to n-1
// This function has to be adapted should input form changes
UnionFind makeUF(int n) {
    UnionFind uf {.nodes = vector<UFNode*>(n), .groupSize = vector<int>(n, 1)};
    for (int i=0; i<n; i++) {
        uf.nodes[i] = new UFNode {i, nullptr};
    }
    return uf;
}

// Given a valid item, finds the group it belongs to while performing path compression along the way
// O(logn) worst case, O(1) average
int Find(UnionFind &uf, int a) {
    vector<UFNode*> v;
    UFNode *cur = uf.nodes[a];
    while (cur->next) {
        v.push_back(cur);
        cur = cur->next;
    }
    for (UFNode *node : v) {
        node->next = cur;
    }
    return cur->val;
}

// Given two valid items, merge the groups they belong to
// O(logn) worst case, O(1) average
void Union(UnionFind &uf, int a, int b) {
    UFNode *first = uf.nodes[Find(uf, a)];
    UFNode *second = uf.nodes[Find(uf, b)];
    // Do nothing if they are in the same group
    if (first == second) {
        return;
    }
    // Merge the smaller group into the larger group
    if (uf.groupSize[first->val] >= uf.groupSize[second->val]) {
        second->next = first;
        uf.groupSize[first->val] += uf.groupSize[second->val];
        uf.groupSize[second->val] = 0;
    } else {
        first->next = second;
        uf.groupSize[second->val] += uf.groupSize[first->val];
        uf.groupSize[first->val] = 0;
    }
}

// ----------------------------------------------------------------------------------------------------------


// KRUSKAL'S MINIMUM SPANNING TREE ALGORITHM ----------------------------------------------------------------

// Requires the above methods to be included
// Takes in a list of weighted edges, each as a pair
// Returns a vector of pairs, each indicating an edge in the MST
// Optimal for sparse graphs
// Runs in O(mlogn) time

// Sort by edge length ascending
bool lenAsc(pair<int,pi> a, pair<int,pi> b) {
    return a.first < b.first;
}

// Each pair in edges is structured as (length, endpoints)
vector<pi> getMST2(int n, vector<pair<int,pi>> edges) {
    vector<pi> MST;
    sort(edges.begin(), edges.end(), lenAsc);
    UnionFind nodes = makeUF(n);
    for (pair<int,pi> edge : edges) {
        // If endpoints not in the same connected component, connect them
        if (Find(nodes, edge.second.first) != Find(nodes, edge.second.second)) {
            Union(nodes, edge.second.first, edge.second.second);
            MST.push_back(edge.second);
        }
    }
    return MST;
}

// ----------------------------------------------------------------------------------------------------------


// MAXIMUM SPACING K-CLUSTERING -----------------------------------------------------------------------------

// Given n nodes and pairwise distances, returns a k-clustering with maximum spacing. That is, we seek to
//  maximize the minumum distance between any pair of nodes belonging to different clusters.
// Based on Kruskal's Algorithm

// Each pair in edges is structured as (length, endpoints)
vector<vector<int>> maxSpaceCluster(vector<pair<int,pi>> edges, int n, int k) {
    // Differs from Kruskal's only in that it stops after adding n-k edges
    vector<vector<int>> clusters;
    sort(edges.begin(), edges.end(), lenAsc);
    UnionFind uf = makeUF(n);
    int numClusters = n;
    for (pair<int,pi> edge : edges) {
        if (numClusters == k) {
            break;
        }
        if (Find(uf, edge.second.first) != Find(uf, edge.second.second)) {
            Union(uf, edge.second.first, edge.second.second);
            numClusters--;
        }
    }
    
    // Generates lists of clusters from union-find
    vector<int> added(n,-1);
    int group;
    int clusInd = 0;
    for (int i=0; i<n; i++) {
        group = Find(uf, i);
        if (added[group] != -1) {
            clusters[added[group]].push_back(i);
        } else {
            clusters.push_back({i});
            added[group] = clusInd++;
        }
    }
    
    return clusters;
}

// ----------------------------------------------------------------------------------------------------------


// TARJAN'S OFF-LINE LOWEST COMMON ANCESTOR ALGORITHM FOR TREES ---------------------------------------------

// Provided a tree of N nodes and a series of Q node pairs, find the LCA of each given pair of nodes.
// Runs in approximately O(N+Q) time

// Prints the LCA of each pair in queries
// The tree is represented by adjacency lists of directed, hierarchal edges. Each node has an edge to its
//  children but not to its parent
void getLCA(tree t, int root, vector<pi> queries) {
    // Two copies of each query is saved.
    vector<vector<int>> req(t.size());
    for (pi q : queries) {
        req[q.first].push_back(q.second);
        req[q.second].push_back(q.first);
    }
    
    // Post-order DFS
    // Idea: the LCA of two nodes belonging to different subtrees rooted at u (including u itself) must be u
    stack<int> s;
    UnionFind uf = makeUF((int)t.size());
    vector<int> explored(t.size()); // 0: unexplored. 1: children pushed to stack. 2: all children explored.
    vector<int> par(t.size(), root); // Records the parent of each node. Let the root's parent be itself
    vector<int> anc(t.size()); // Current ancestor
    s.push(root);
    int cur;
    while (!s.empty()) {
        cur = s.top();
        if (explored[cur] == 0) {
            for (int child : t[cur]) {
                s.push(child);
                par[child] = cur;
            }
            explored[cur]++;
        } else {
        // Since trees contain no cycles, each node will only be pushed once. Thus, the case where cur has
        //  already been popped (explored[cur] == 2) need not be considered.
            for (int target : req[cur]) {
                // Answer each query the second time it is encountered
                if (explored[target] == 2) {
                    cout << "LCM of " << cur << " and " << target << " is " << anc[Find(uf, target)] << endl;
                }
            }
            Union(uf, par[cur], cur); // Merging up
            anc[Find(uf, cur)] = par[cur]; // Common ancestor moves up one layer
            explored[cur]++;
            s.pop();
        }
    }
}

// ----------------------------------------------------------------------------------------------------------

