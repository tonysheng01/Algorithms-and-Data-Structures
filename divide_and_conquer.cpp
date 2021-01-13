// Representative Divide and Conquer Algorithms

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

typedef pair<double,double> point;


// 0. BINARY SEARCH --------------------------------------------------------------------------------------------

// T(n) = T(n/2) + O(1) => T(n) = O(logn)

// Given an array of distinct integers sorted in ascending order and a target, returns the index of the target
//  in the array or -1 if it does not exist.

int search(vector<int> nums, int target) {
    int lo = 0;
    int hi = v.size() - 1;
    int mid;
    while (lo <= hi) {
        mid = lo + (hi-lo) / 2;
        if (nums[mid] == target) {
            return mid;
        } else if (nums[mid] < target) {
            lo = mid + 1;
        } else { // nums[mid] > target
            hi = mid - 1;
        }
    }
    return -1;
}

// -------------------------------------------------------------------------------------------------------------


// 1. MERGE SORT AND COUNTING INVERSIONS -----------------------------------------------------------------------

// T(n) = 2T(n/2) + O(n) => T(n) = O(nlogn)

// Given a vector v and bounds such that the two subarrays v[l:m-1] and v[m:r-1] are already sorted, merges
//  them into a larger sorted subarray v[l:r-1] and counts inversions with one element in the left subpart and
//  the other in the right subpart (cross inversions)
int mergeAndCount(vector<int> &v, int l, int r, int m) {
    vector<int> temp;
    int inv = 0;
    int lcur = l;
    int rcur = m;
    while (lcur < m || rcur < r) {
        // Note that a pair of duplicate elements does not form an inversion
        if (rcur == r || (lcur < m && v[lcur] <= v[rcur])) {
            temp.push_back(v[lcur]);
            lcur++;
        } else {
        // When an element from the right subpart is selected, we know it forms inversions with all remaining
        //  elements in the left subpart
            temp.push_back(v[rcur]);
            inv += (m - lcur);
            rcur++;
        }
    }
    for (int i=l, j=0; i<r; i++, j++) {
        v[i] = temp[j];
    }
    return inv;
}

// Given a vector v with bounds l and r, sort the subarray v[l:r-1] and returns the number of inversions in it
// If operating on entire vector, set l = 0 and r = v.size()
int sortAndCount(vector<int> &v, int l, int r) {
    if (r == l + 1) {
        return 0;
    }
    int m = l + (r-l)/2;
    // count(all) = count(leftPart) + count(rightPart) + count(crossInversions)
    return sortAndCount(v, l, m) + sortAndCount(v, m, r) + mergeAndCount(v, l, r, m);
}

// ----------------------------------------------------------------------------------------------------------


// 2. CLOSEST PAIR OF POINTS --------------------------------------------------------------------------------

// T(n) = 2T(n/2) + O(n) => T(n) = O(nlogn)
// sqrt-optimized

// Given n points in 2D space represented by (x,y) pairs , finds the distance between the closest pair of points

// Compare by x-coordinate
bool compByX(point a, point b) {
    return a.first < b.first;
}

// Compare by y-coordinate
bool compByY(point a, point b) {
    return a.second < b.second;
}

// Calculate square of distance between two points
double disSquared(point a, point b) {
    return pow(b.first-a.first, 2) + pow(b.second-a.second, 2);
}

// Recursive auxillary function (that does most of the work)
// Takes in two arrays Px and Py that represent the same set of points P sorted by x and y respectively
// Returns the square of distance between the closest points
double MDSRec(vector<point> px, vector<point> py) {
    
    // Base cases
    if (px.size() == 2) {
        return disSquared(px[0], px[1]);
    }
    if (px.size() == 3) {
        return min(disSquared(px[0], px[1]), min(disSquared(px[0], px[2]), disSquared(px[1], px[2])));
    }
    
    // Divides the set P into Q (left half) and R (right half) based on x-cord
    int mid = px.size() / 2;
    double qmax = px[mid-1].first; // x-cord of the rightmost point in Q
    vector<point> qx, qy, rx, ry; // Q = Px[0:mid-1], R = Px[mid:len-1]
    for (int i=0; i<mid; i++) {
        qx.push_back(px[i]);
    }
    for (int i=mid; i<px.size(); i++) {
        rx.push_back(px[i]);
    }
    // Qy and Ry are the respectively sets Q and R sorted by y-cord
    for (point p : py) {
        if (p.first <= qmax) {
            qy.push_back(p);
        } else {
            ry.push_back(p);
        }
    }
    // Recursively computes the closest distance between pairs (a,b) with a and b both in Q or both in R
    double deltaSquared = min(MDSRec(qx, qy), MDSRec(rx, ry));
    
    double minDisSquared = deltaSquared;
    double delta = sqrt(deltaSquared);
    // Sy is the set of all points within distance delta from the vertical line x = qmax, sorted by y-cord
    vector<point> sy;
    for (point p : py) {
        if (abs(p.first - qmax) < delta) {
            sy.push_back(p);
        }
    }
    // Computes the closest distance between pairs (a,b) with a and b belonging to different subsets (Q or R)
    for (int i=0; i<sy.size(); i++) {
        // For each point in Sy, it suffices to consider a maximum of 7 points above it
        for (int j=i+1; j<i+7 && j<sy.size(); j++) {
            minDisSquared = min(minDisSquared, disSquared(sy[i], sy[j]));
        }
    }
    
    return minDisSquared;
}

// Uses MDSRec to compute the minimum pairwise distance given a set of points
double minPairwiseDistance(vector<point> points) {
    vector<point> px = points;
    vector<point> py = points;
    sort(px.begin(), px.end(), compByX);
    sort(py.begin(), py.end(), compByY);
    return sqrt(MDSRec(px, py));
}

// ----------------------------------------------------------------------------------------------------------

