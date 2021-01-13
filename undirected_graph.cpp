// Algorithms related to undirected graphs
// Graphs are represented with adjacency lists with n nodes as distinct ints from 0 to n-1

#include <vector>
#include <stack>
#include <queue>
#include <iostream>

using namespace std;

typedef vector<vector<int>> graph; // Adjacency Lists
typedef pair<int,int> pi;
typedef vector<vector<pi>> weighted_graph; // (node,distance) pairs

// 1. BFS --------------------------------------------------------------------------------------------------

// Abstracted BFS for a single connected component in a graph
// Prints all nodes reachable from root in BFS order

void BFS (graph l, int root) {
    vector<int> discovered(l.size(),0); // 0 or 1
    queue<int> q;
    q.push(root);
    discovered[root] = 1;
    while (!q.empty()) {
        for (int node : l[q.front()]) {
            if (!discovered[node]) {
                q.push(node);
                discovered[node] = 1;
            }
        }
        cout << q.front() << endl; // Can be replaced with necessary node operations
        q.pop();
    }
}

// ----------------------------------------------------------------------------------------------------------


// 2. DFS ---------------------------------------------------------------------------------------------------

// Abstracted iterative DFS for a single connected component in a graph
// Prints all reachable nodes in DFS order starting with root

void DFS (graph l, int root) {
    vector<int> explored(l.size(),0); // 0 or 1
    stack<int> s;
    s.push(root);
    int cur = root;
    while (!s.empty()) {
        cur = s.top();
        s.pop();
        if (explored[cur]) {
            continue;
        }
        explored[cur] = 1;
        cout << cur << endl; // Can be replaced with necessary node operations
        for (int node : l[cur]) {
            if (!explored[node]) {
                s.push(node);
            }
        }
    }
}

// ----------------------------------------------------------------------------------------------------------


// 3. TESTING BIPARTITENESS ---------------------------------------------------------------------------------

// Given an undirected graph represented by adjacency lists, returns whether or not it is bipartite

bool isBipartite(graph g) {
    queue<int> q;
    vector<int> group(g.size()); // Each node is assigned 1 or -1
    vector<int> discovered(g.size());
    int cur;
    
    // BFS
    for (int i=0; i<g.size(); i++) {
        if (discovered[i]) {
            continue;
        }
        q.push(i);
        group[i] = 1;
        discovered[i] = 1;
        while (!q.empty()) {
            cur = q.front();
            q.pop();
            for (int node : g[cur]) {
                if (discovered[node] == 0) {
                    q.push(node);
                    group[node] = -group[cur]; // Assigning each child of cur the opposite group of cur
                    discovered[node] = 1;
                }
            }
        }
    }
    
    // Detecting edges with both ends in the same group
    for (int i=0; i<g.size(); i++) {
        for (int j : g[i]) {
            if (group[i] == group[j]) {
                return false;
            }
        }
    }
    
    return true;
}

// ----------------------------------------------------------------------------------------------------------


// 4. CONNECTED COMPONENTS ----------------------------------------------------------------------------------

// Finds the set of all connected components in an undirected graph using BFS

vector<vector<int>> connectedComp (graph l) {
    vector<vector<int>> ans;
    vector<int> cur;
    vector<int> discovered(l.size(),0); // 0 or 1
    queue<int> q;
    
    for (int i=0; i<l.size(); i++) {
        if (discovered[i]) {
            continue;
        }
        
        // BFS starting from node i
        discovered[i] = 1;
        q.push(i);
        while (!q.empty()) {
            for (int node : l[q.front()]) {
                if (!discovered[node]) {
                    discovered[node] = 1;
                    q.push(node);
                }
            }
            cur.push_back(q.front());
            q.pop();
        }
        
        ans.push_back(cur);
        cur.clear();
    }
    
    return ans;
}

// ----------------------------------------------------------------------------------------------------------


// 5. TARJAN'S ALGORITHM FOR FINDING ARTICULATION POINTS ----------------------------------------------------

// Returns all articulation points in a graph

vector<int> getAP (graph g) {
    vector<int> ap(g.size()); // Records whether or not a node is an AP
    vector<int> ans; // Records all APs
    vector<int> exp(g.size()); // DFS exploring order, starts with 1
    // For any node A, low = min(x,y)
    // x = the order of earliest-visited node reachable from the subtree rooted at A
    // y = the order of earliest-visited non-parent ancestor that has a back edge from A
    vector<int> low(g.size()); // Earliest-visited node reachable from
    vector<int> cleared(g.size()); // Whether the recursive call from a node has returned
    vector<int> parent(g.size(),-1); // Parent of each node in DFS tree
    stack<int> s;
    int order = 0;
    int cur;
    
    for (int i=0; i<g.size(); i++) {
        if (cleared[i]) {
            continue;
        }
        
        s.push(i);
        while (!s.empty()) {
            cur = s.top();
            if (cleared[cur]) {
                s.pop();
                continue;
            }
            
            if (exp[cur] == 0) {
                order++;
                exp[cur] = order;
                low[cur] = order;
                for (int node : g[cur]) {
                    if (exp[node] == 0) {
                        s.push(node);
                        parent[node] = cur;
                    }
                }
            } else {
                // If cur is not the root of current DFS tree,
                // cur is an AP iff it has a child that can only be reached through cur,
                // which is demostrated by low[child]>=low[cur]
                if (parent[cur] != -1) {
                    for (int node : g[cur]) {
                        if (exp[node] > exp[cur]) {
                            // If node is cur's child
                            low[cur] = min(low[node],low[cur]);
                            if (low[node] >= exp[cur]) {
                                ap[cur] = 1;
                            }
                        } else if (node != parent[cur]) {
                            // If node is cur's non-parent ancestor
                            low[cur] = min(low[cur],exp[node]);
                        }
                    }
                } else {
                // If cur is the root, cur is an AP iff cur has more than one disjoint subtrees
                    ap[cur] = count(parent.begin(),parent.end(),cur)>1 ? 1 : 0;
                }
                cleared[cur] = 1;
                s.pop();
            }
        }
    }
    
    for (int i=0; i<g.size(); i++) {
        if (ap[i]) {
            ans.push_back(i);
        }
    }
    
    return ans;
}

// ----------------------------------------------------------------------------------------------------------


// 6. TARJAN'S ALGORITHM FOR FINDING BRIDGES ----------------------------------------------------------------

// Returns a vector of pairs where each pair contains the endpoints of a bridge

vector<pair<int,int>> getBridges (graph g) {
    vector<pair<int,int>> bridges;
    vector<int> exp(g.size());
    vector<int> low(g.size());
    vector<int> parent(g.size(),-1);
    vector<int> cleared(g.size());
    stack<int> s;
    int order = 0;
    int cur;
    
    for (int i=0; i<g.size(); i++) {
        if (cleared[i]) {
            continue;
        }
        
        s.push(i);
        while (!s.empty()) {
            cur = s.top();
            if (cleared[cur]) {
                s.pop();
                continue;
            }
            
            if (exp[cur] == 0) {
                order++;
                exp[cur] = order;
                low[cur] = order;
                for (int node : g[cur]) {
                    if (exp[node] == 0) {
                        parent[node] = cur;
                        s.push(node);
                    }
                }
            } else {
                // Whether or not cur is the root is irrelevant
                for (int node : g[cur]) {
                    if (exp[node] > exp[cur]) {
                        low[cur] = min(low[cur],low[node]);
                        // cur-node is a bridge iff low[node]>exp[cur].Note that equality does not imply a bridge.
                        if (low[node] > exp[cur]) {
                            bridges.push_back(make_pair(cur, node));
                        }
                    } else if (parent[cur] != node) {
                        low[cur] = min(low[cur],exp[node]);
                    }
                }
                cleared[cur] = 1;
                s.pop();
            }
        }
    }
    
    return bridges;
}

// ----------------------------------------------------------------------------------------------------------


// 7. PRIM'S MINIMUM SPANNING TREE ALGORITHM ----------------------------------------------------------------

// Returns a vector of pairs, each indicating an edge in the MST
// Optimal for dense graphs
// Runs in O(mlogn) time

vector<pi> getMST(weighted_graph g) {
    vector<pi> MST;
    vector<int> explored(g.size());
    vector<int> minLen(g.size(), -1); // Length of shortest edge from an explored node to the current node
    // Each pair denotes an edge. Pair structure: (len, (start, end))
    priority_queue<pair<int,pi>, vector<pair<int,pi>>, greater<pair<int,pi>>> pq;
    pq.push(make_pair(0, make_pair(0, 0)));
    pi curEdge;
    
    // In each iteration, add a node not yet explored with the minimum minLen to the set and add the shortest
    //  edge connecting it from an explored node to the MST
    while (!pq.empty()) {
        curEdge = pq.top().second;
        pq.pop();
        if (explored[curEdge.second]) {
            continue;
        }
        for (pi p : g[curEdge.second]) {
            if (minLen[p.first] == -1 || p.second < minLen[p.first]) {
                minLen[p.first] = p.second;
                pq.push(make_pair(p.second, make_pair(curEdge.second, p.first)));
            }
        }
        explored[curEdge.second] = 1;
        MST.push_back(curEdge);
    }
    
    MST.erase(MST.begin()); // The self-edge (0,0) will always be first in the resulting vector. Exclude it.
    return MST;
}

// ----------------------------------------------------------------------------------------------------------


// 8. KRUSKAL'S MINIMUM SPANNING TREE ALGORITHM -------------------------------------------------------------

// Takes in a list of weighted edges as opposed to adjacency lists
// Returns a vector of pairs, each indicating an edge in the MST
// Optimal for sparse graphs
// Runs in O(mlogn) time
// Refer to the union-find template for implementation

// ----------------------------------------------------------------------------------------------------------


// 9. MAXIMUM SPACING K-CLUSTERING --------------------------------------------------------------------------

// Given n nodes and pairwise distances, returns a k-clustering with maximum spacing. That is, we seek to
//  maximize the minumum distance between any pair of nodes belonging to different clusters.
// Based on Kruskal's Algorithm
// Refer to the union-find template for implementation

// ----------------------------------------------------------------------------------------------------------
