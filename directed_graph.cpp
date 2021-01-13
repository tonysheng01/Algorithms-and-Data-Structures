// Algorithms related to directed graphs
// Graphs are represented with adjacency lists with n nodes as distinct ints from 0 to n-1

#include <iostream>
#include <vector>
#include <stack>
#include <queue>

using namespace std;

typedef vector<vector<int>> graph; // Adjacency lists
typedef pair<int,int> pi;
typedef vector<vector<pi>> weighted_graph; // (node,distance) pairs

// 1. TOPOLOGICAL SORTING ---------------------------------------------------------------------------------

// Returns a topological order of a DAG, or an empty vector if the input is not a DAG
// The graph is not required to be connected

vector<int> topologicalOrder (adjList l) {
    vector<int> to;
    vector<int> activePre(l.size());
    queue<int> leadingNodes;
    // Preprocessing
    for (vector<int> v : l) {
        for (int node : v) {
            activePre[node]++;
        }
    }
    for (int i=0; i<l.size(); i++) {
        if (activePre[i] == 0) {
            leadingNodes.push(i);
        }
    }
    // Extracting nodes
    int cur;
    while (!leadingNodes.empty()) {
        cur = leadingNodes.front();
        to.push_back(cur);
        for (int node : l[cur]) {
            activePre[node]--;
            if (activePre[node] == 0) {
                leadingNodes.push(node);
            }
        }
        leadingNodes.pop();
    }
    // Checking against cycles
    if (to.size() < l.size()) {
        return {};
    }
    return to;
}

// ----------------------------------------------------------------------------------------------------------


// 2. KOSARAJU'S ALGORITHM FOR STRONGLY CONNECTED COMPONENTS ------------------------------------------------

// Returns a vector of vectors, each of which contains an SCC
// Iterative version

vector<vector<int>> getSCC (graph g) {
    
    // A slightly modified DFS procedure on g to order nodes by finishing time
    stack<int> s;
    stack<int> f; // finishing time in descending order
    // meaning of status:
    // 0: unexplored
    // 1: all unexplored(0) children pushed to stack (recursive call started)
    // 2: all children explored(>0) (recursive call returned)
    vector<int> status(g.size());
    int cur;
    for (int i=0; i<g.size(); i++) {
        if (status[i] > 0) { // more strictly, status[i] is either 0 or 2
            continue;
        }
        
        s.push(i);
        while (!s.empty()) {
            cur = s.top();
            if (status[cur] == 0) {
                for (int node : g[cur]) {
                    if (status[node] == 0) {
                        s.push(node);
                    }
                }
                status[cur]++;
            } else if (status[cur] == 1) {
                f.push(cur);
                s.pop();
                status[cur]++;
            } else {
                s.pop();
            }
        }
    }
    
    // Create transpose graph t
    graph t(g.size());
    for (int i=0; i<g.size(); i++) {
        for (int node : g[i]) {
            t[node].push_back(i);
        }
    }
    
    // For each node in f, DFS on t to find a strongly connected component
    vector<vector<int>> scc;
    vector<int> curSet;
    vector<int> explored(t.size()); // 0 or 1 for ordinary dfs
    int root;
    while (!f.empty()) {
        root = f.top();
        f.pop();
        if (explored[root]) {
            continue;
        }
        
        s.push(root);
        while (!s.empty()) {
            cur = s.top();
            s.pop();
            if (explored[cur]) {
                continue;
            }
            explored[cur] = 1;
            curSet.push_back(cur);
            for (int node : t[cur]) {
                if (!explored[node]) {
                    s.push(node);
                }
            }
        }
        
        scc.push_back(curSet);
        curSet.clear();
    }
    
    return scc;
}

// ----------------------------------------------------------------------------------------------------------


// 3. TARJAN'S ALGORITHM FOR STRONGLY CONNECTED COMPONENTS --------------------------------------------------

// Returns a vector of vectors, each of which contains an SCC
// Iterative version

vector<vector<int>> getSCC2 (graph g) {
    vector<vector<int>> SCC;
    vector<int> curSet;
    stack<int> s1; // Standard DFS stack
    stack<int> s2; // Stack for grouping SCCs. Elements are pushed in the same order as they are explored.
    vector<int> exp(g.size()); // The visiting order of each node
    vector<int> low(g.size()); // The earliest-visited node reachable from each node
    vector<int> cleared(g.size()); // 1 if a node has already been idenfitied as part of an SCC
    int cur;
    int order = 0; // Visiting order
    
    for (int i=0; i<g.size(); i++) {
        if (cleared[i]) {
            continue;
        }
        
        s1.push(i);
        while (!s1.empty()) {
            cur = s1.top();
            if (cleared[cur]) {
                s1.pop();
                continue;
            }
            
            if (exp[cur] == 0) {
                order++;
                exp[cur] = order;
                low[cur] = order;
                s2.push(cur);
                for (int child : g[cur]) {
                    if (exp[child] == 0) {
                        s1.push(child);
                    }
                }
            } else {
                for (int child : g[cur]) {
                    // If child is cleared, there cannot be a path from child to cur
                    if (!cleared[child] && low[child] < low[cur]) {
                        low[cur] = low[child];
                    }
                }
                s1.pop();
                // Nodes with equal low values are in the same SCC
                if (low[cur] == exp[cur]) {
                    while (s2.top() != cur) {
                        curSet.push_back(s2.top());
                        cleared[s2.top()] = 1;
                        s2.pop();
                    }
                    curSet.push_back(cur);
                    cleared[cur] = 1;
                    s2.pop();
                    SCC.push_back(curSet);
                    curSet.clear();
                }
            }
        }
    }
    
    return SCC;
}

// ----------------------------------------------------------------------------------------------------------


// 4. DIJKSTRA'S ALGORITHM FOR SHORTEST PATHS FROM A POINT --------------------------------------------------

// Given a directed or undirected graph g with non-negative edge weights and a source node, returns a vector
//  whose i-th index holds the shortest distance from the source to the i-th node.
// Runs in O(mlogn) time

vector<int> shortestDistance(weighted_graph g, int source) {
    vector<int> explored(g.size());
    vector<int> shortestDistance(g.size(), -1);
    // pi.second is a node i and p.first is the shortest distance from source to i based on current explored set
    // pq may contain duplicate nodes but will be properly dealt with in the main loop
    priority_queue<pi,vector<pi>,greater<pi>> pq;
    shortestDistance[source] = 0;
    pq.push(make_pair(0, source));
    int cur;
    
    // In each iteration, extract the unexplored node with smallest distance from source, add it to the explored
    //  set and push its still unexplored children for consideration
    while (!pq.empty()) {
        cur = pq.top().second;
        pq.pop();
        if (explored[cur]) {
            continue;
        }
        for (pi p : g[cur]) {
            if (shortestDistance[p.first] == -1 || shortestDistance[cur] + p.second < shortestDistance[p.first]) {
                // Update shortest distance based on current explored set
                shortestDistance[p.first] = shortestDistance[cur] + p.second;
                pq.push(make_pair(shortestDistance[p.first], p.first));
            }
        }
        explored[cur] = 1;
    }
    
    return shortestDistance;
}

// ----------------------------------------------------------------------------------------------------------


// 5. TARJAN'S OFF-LINE LOWEST COMMON ANCESTOR ALGORITHM FOR TREES ------------------------------------------

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
                    cout << "LCA of " << cur << " and " << target << " is " << anc[Find(uf, target)] << endl;
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
