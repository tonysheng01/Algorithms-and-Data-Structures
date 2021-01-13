// Representative Greedy Algorithms

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <map>

using namespace std;

typedef pair<int,int> pi;

// -----------------------------------------------------------------------------------------------------
// INTERVAL SCHEDULING

// Finds a conflict-free schedule holidng the most number of events
// Runs in O(nlogn) time, or O(n) if input is already sorted
// -----------------------------------------------------------------------------------------------------

// Comparing by end time ascending
bool endTimeAsc(pi a, pi b) {
    return a.second < b.second;
}

// Each event is represented by a pair consisting of its start and finish time
void getMaxSchedule(vector<pi> schedule) {
    sort(schedule.begin(),schedule.end(),endTimeAsc);
    int lastEnd = -1;
    // Adds the event of the earliest end time while avoiding conflict
    for (pi event : schedule) {
        if (event.first >= lastEnd) {
            cout << event.first << " " << event.second << endl;
            lastEnd = event.second;
        }
    }
}


// -----------------------------------------------------------------------------------------------------
// INTERVAL PARTITIONING

// Partitions all events into the least number of groups, each of which has a non-conflicting schedule
// Runs in O(nlogn) time
// -----------------------------------------------------------------------------------------------------

// Comparing by start time ascending
bool startTimeAsc(pi a, pi b) {
    return a.first < b.first;
}

// Each event is represented by a pair consisting of its start and finish time
vector<vector<pi>> getLeastPartition(vector<pi> schedule) {
    int group;
    int numGroup = 0;
    vector<int> lastEnd;
    vector<vector<pi>> partition;
    // Priority queue in ascending order of latest endtime
    // For each pair, second is the group number and first is the endtime of the last event in the group
    priority_queue<pi, vector<pi>, greater<pi>> earliestFinish;
    sort(schedule.begin(), schedule.end(), startTimeAsc);
    for (pi event : schedule) {
        // If there is no group created or if the earliest endtimes of last events in all groups
        //  are after the current event's start time, create a new group
        if (numGroup == 0 || earliestFinish.top().first > event.first) {
            partition.push_back(vector<pi> {event});
            earliestFinish.push(make_pair(event.second,numGroup++));
        } else {
        // Push the current event into the non-conflicting group with earliest endtime,
        //  and update the position of current group in priority queue
            group = earliestFinish.top().second;
            partition[group].push_back(event);
            earliestFinish.pop();
            earliestFinish.push(make_pair(event.second,group));
        }
    }
    return partition;
}


// -----------------------------------------------------------------------------------------------------
// MINIMIZING MAX LATENESS

// Given a start time and a set of events where each event has a duration and a deadline, we fit all
//  events in a schedule, despite some of them running late. Define the lateness l(i) of an event i to
//  be its finish time in the schedule minus its deadline, or 0 if finished in time. The max lateness of
//  a given schedule is max(l(i)) for all events i. We seek a schedule that minimizes the max lateness.
// Runs in O(nlogn) time, or O(n) if input is already sorted
// -----------------------------------------------------------------------------------------------------

// Comparing by deadline ascending
bool deadlineAsc(pi a, pi b) {
    return a.second < b.second;
}

// Finds a schedule with the minimum max lateness and returns its value.
// Each event is represented by a pair consisting of its duration and deadline
int minMaxLateness(vector<pi> events, int startTime) {
    sort(events.begin(), events.end(), deadlineAsc);
    int maxLateness = 0;
    int curStart = startTime;
    // In each iteration, add the item in the list with the earliest deadline
    for (pi event : events) {
        maxLateness = max(maxLateness, curStart + event.first - event.second);
        curStart += event.first;
    }
    return maxLateness;
}


// -----------------------------------------------------------------------------------------------------
// OPTIMAL CACHING - FARTHEST IN FUTURE APPROACH

// Given a sequence of n items and a cache of size k<=n which may initially contain some items, we need
//  to access each item in the order of the sequence. If the current item i is not in the cache and the
//  cache is full, we evict an existing item to leave space for i. Such an action is defined as a cache
//  miss. Find an eviction schedule that results in the minimum number of cache misses.
// Runs in O(nlogn) time
// -----------------------------------------------------------------------------------------------------

// Returns the minimum cache misses given a sequence of items to be cached, an list of items initially
//  in the cache and the cache capacity
// Each item is represented by a unique int
int minCacheMiss(vector<int> items, vector<int> initialCache, int cacheCap) {
    int n = (int)items.size();
    int cacheSize = (int)initialCache.size();
    map<int,bool> inCache;
    // The map stores the indexes at which each item is called
    map<int,queue<int>> calls;
    // The pq stores items currently in cache in descending order of next call time.
    // May contain duplicate items but does not affect the result.
    priority_queue<pi> cache;
    int cur;
    int miss = 0;
    for (int i=0; i<items.size(); i++) {
        cur = items[i];
        inCache[cur] = false;
        calls[cur].push(i);
    }
    for (int item : items) {
        calls[item].push(n);
    }
    for (int item : initialCache) {
        inCache[item] = true;
        cache.push(make_pair(calls[item].front(), item));
    }
    for (int item : items) {
        if (!inCache[item]) {
            inCache[item] = true;
            if (cacheSize < cacheCap) {
                cacheSize++;
            } else {
                // When the currently fetched item is not in cache and the cache is full, evict the item in
                //  cache whose next call is farthest in future to give space for current item
                inCache[cache.top().second] = false;
                cache.pop();
                miss++;
            }
        }
        // Updates current item's next call time, whether or not a cache miss occurs
        calls[item].pop();
        cache.push(make_pair(calls[item].front(), item));
    }
    return miss;
}
