// Binary min-heap implemented with vectors
// Can be easily converted to max-heaps

#include <vector>
#include <iostream>

using namespace std;

typedef vector<int> heap;

void swap (heap &h, int i, int j) {
    int a = h[i-1];
    h[i-1] = h[j-1];
    h[j-1] = a;
}

void heapify_up (heap &h, int i) {
    if (i > 1) {
        int p = i/2;
        if (h[p-1] > h[i-1]) {
            swap(h, i, p);
            heapify_up(h, p);
        }
    }
}

void heapify_down (heap &h, int i) {
    int n = int(h.size());
    int j;
    if (2*i == n) {
        j = n;
    } else if (2*i < n) {
        j = h[2*i-1]>h[2*i] ? (2*i+1) : (2*i);
    } else {
        return;
    }
    if (h[i-1] > h[j-1]) {
        swap(h, i, j);
        heapify_down(h, j);
    }
}

void insert (heap &h, int k) {
    int n = int(h.size());
    h.push_back(k);
    heapify_up(h, n+1);
}

int findMin (const heap &h) {
    return h[0];
}

void remove (heap &h, int i) {
    h[i-1] = h.back();
    h.pop_back();
    heapify_up(h, i);
    heapify_down(h, i);
}

int extractMin (heap &h) {
    int a = findMin(h);
    remove(h, 1);
    return a;
}

void printHeap (const heap &h) {
    for (int i : h) {
        cout << i << endl;
    }
}

void printSorted (heap &h) {
    while (h.size() > 0) {
        cout << extractMin(h) << endl;
    }
}
