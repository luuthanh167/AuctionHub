#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;
using namespace std::chrono;

struct Bid {
    string bidderId;
    double amount;       // bid price
    long long timestamp; // Unix ms
    int auctionId;
};

// ---------- Utilities ----------
void printBids(const vector<Bid>& v, size_t limit = 10) {
    for (size_t i = 0; i < min(v.size(), limit); ++i)
        cout << "[" << v[i].bidderId << "] $" << v[i].amount
             << " @ " << v[i].timestamp << "\n";
    if (v.size() > limit) cout << "... (" << v.size() << " total)\n";
}

bool loadBids(const string& path, vector<Bid>& out) {
    out.clear();
    ifstream f(path);
    if (!f) return false;
    Bid b;
    while (f >> b.bidderId >> b.amount >> b.timestamp >> b.auctionId)
        out.push_back(b);
    return true;
}

void generateLargeDataset(const string& path, int count = 100000) {
    ofstream out(path);
    mt19937 rng(42);
    uniform_real_distribution<double> priceDist(10.0, 5000.0);
    uniform_int_distribution<long long> timeDist(1700000000000LL, 1700086400000LL);
    uniform_int_distribution<int> auctionDist(1, 500);

    for (int i = 0; i < count; ++i) {
        out << "user_" << (i % 10000) << " "
            << priceDist(rng) << " "
            << timeDist(rng) << " "
            << auctionDist(rng) << "\n";
    }
}

bool isSortedAscending(const vector<Bid>& v) {
    for (size_t i = 1; i < v.size(); ++i) {
        if (v[i].amount < v[i - 1].amount) return false;
        if (v[i].amount == v[i - 1].amount && v[i].timestamp < v[i - 1].timestamp) return false;
    }
    return true;
}

// Hàm phụ trợ so sánh tăng dần: ưu tiên amount, nếu bằng nhau thì xét timestamp
bool comesBefore(const Bid& a, const Bid& b) {
    if (a.amount != b.amount) return a.amount < b.amount;
    return a.timestamp < b.timestamp;
}

// ---------- Task A: Insertion Sort (từ insertion_sort.c) ----------
void insertionSort(vector<Bid>& v) {
    int n = static_cast<int>(v.size());
    for (int i = 1; i < n; i++) {
        Bid key = v[i];
        int j = i - 1;
        while (j >= 0 && comesBefore(key, v[j])) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = key;
    }
}

// ---------- Task B: Selection Sort giảm dần (từ sort_comparison.c) ----------
void selectionSort(vector<Bid>& v) {
    int n = static_cast<int>(v.size());
    for (int i = 0; i < n - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < n; j++) {
            // Sắp xếp giảm dần theo amount (nếu bằng thì timestamp nhỏ hơn đứng trước)
            if (v[j].amount > v[maxIdx].amount ||
               (v[j].amount == v[maxIdx].amount && v[j].timestamp < v[maxIdx].timestamp)) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            swap(v[i], v[maxIdx]);
        }
    }
}

// ---------- Task C: Interchange Sort & đếm tổng swap ----------
void interchangeSort(vector<Bid>& v) {
    int n = static_cast<int>(v.size());
    long long swapCount = 0;
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (comesBefore(v[j], v[i])) {
                swap(v[i], v[j]);
                swapCount++;
            }
        }
    }
    cout << "Total swaps performed: " << swapCount << "\n";
}

// ---------- Task D: Bubble Sort Early Stop (từ bubble_sort_comparison.c) ----------
bool bubbleSortEarlyStop(vector<Bid>& v) {
    int n = static_cast<int>(v.size());
    bool isStableFirstPass = true;

    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; j++) {
            if (comesBefore(v[j + 1], v[j])) {
                swap(v[j], v[j + 1]);
                swapped = true;
            }
        }
        if (i == 0 && swapped) {
            isStableFirstPass = false;
        }
        if (!swapped) {
            break;
        }
    }
    return isStableFirstPass;
}

// ---------- Task E: Quick Sort Median-of-Three (từ Median-of-three Pivot.c) ----------
int medianOfThree(const vector<Bid>& v, int lo, int hi) {
    int mid = lo + (hi - lo) / 2;

    if ((comesBefore(v[lo], v[mid]) && comesBefore(v[mid], v[hi])) ||
        (comesBefore(v[hi], v[mid]) && comesBefore(v[mid], v[lo]))) {
        return mid;
    }

    if ((comesBefore(v[mid], v[lo]) && comesBefore(v[lo], v[hi])) ||
        (comesBefore(v[hi], v[lo]) && comesBefore(v[lo], v[mid]))) {
        return lo;
    }

    return hi;
}

int partition(vector<Bid>& v, int lo, int hi) {
    int pivotIndex = medianOfThree(v, lo, hi);
    swap(v[pivotIndex], v[hi]);

    Bid pivot = v[hi];
    int i = lo - 1;

    for (int j = lo; j < hi; j++) {
        if (!comesBefore(pivot, v[j])) { // tương đương v[j] <= pivot
            i++;
            swap(v[i], v[j]);
        }
    }
    swap(v[i + 1], v[hi]);
    return i + 1;
}

void quickSort(vector<Bid>& v, int lo, int hi) {
    if (lo < hi) {
        int pi = partition(v, lo, hi);
        quickSort(v, lo, pi - 1);
        quickSort(v, pi + 1, hi);
    }
}

void quickSortWrapper(vector<Bid>& v) {
    if (v.size() <= 1) return;
    quickSort(v, 0, static_cast<int>(v.size()) - 1);
}

// ---------- Driver ----------
int main() {
    vector<Bid> smallData;
    if (!loadBids("test_data/small.txt", smallData)) {
        cerr << "Khong tim thay test_data/small.txt!\n";
        return 1;
    }

    cout << "=== Task A: Insertion Sort ===\n";
    vector<Bid> dataA = smallData;
    insertionSort(dataA);
    printBids(dataA);

    cout << "\n=== Task B: Selection Sort (Top 3) ===\n";
    vector<Bid> dataB = smallData;
    selectionSort(dataB);
    printBids(dataB, 3);

    cout << "\n=== Task C: Interchange Sort ===\n";
    vector<Bid> dataC = smallData;
    interchangeSort(dataC);
    printBids(dataC);

    cout << "\n=== Task D: Bubble Sort Early Stop ===\n";
    vector<Bid> dataD1 = dataA;
    cout << "Stream 1: " << (bubbleSortEarlyStop(dataD1) ? "STABLE" : "UNSTABLE") << "\n";
    vector<Bid> dataD2 = dataB;
    cout << "Stream 2: " << (bubbleSortEarlyStop(dataD2) ? "STABLE" : "UNSTABLE") << "\n";

    cout << "\n=== Task E: Quick Sort (100,000 bids) ===\n";
    generateLargeDataset("test_data/large.txt", 100000);
    vector<Bid> largeData;
    loadBids("test_data/large.txt", largeData);

    auto start = high_resolution_clock::now();
    quickSortWrapper(largeData);
    auto stop = high_resolution_clock::now();
    double ms = duration_cast<microseconds>(stop - start).count() / 1000.0;

    cout << "Sorted check: " << (isSortedAscending(largeData) ? "PASS" : "FAIL") << "\n";
    cout << "Runtime (100k records): " << ms << " ms\n";

    return 0;
}