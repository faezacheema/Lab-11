#include <iostream>
using namespace std;

// ---- Globals used ONLY for tracing, so the sort functions keep the exact
// ---- signatures required by the handout.
static bool g_trace = true;     // print [low..high] + whole array after each merge
static int* g_traceArr = nullptr;  // the array being sorted (for printing it whole)
static int  g_traceN = 0;        // its length
static int  g_mergeCalls = 0;        // how many times merge() ran

// Print arr[0..n-1] as {a, b, c}
void printArray(const int arr[], int n) {
    cout << "{";
    for (int i = 0; i < n; i++) {
        cout << arr[i];
        if (i + 1 < n) cout << ", ";
    }
    cout << "}";
}

// ----------------------------------------------------------------------------
// merge: merges the two adjacent SORTED ranges arr[low..mid] and
//        arr[mid+1..high] into one sorted range arr[low..high].
// ----------------------------------------------------------------------------
void merge(int arr[], int low, int mid, int high) {
    int n1 = mid - low + 1;   // size of left half  (low .. mid)
    int n2 = high - mid;      // size of right half (mid+1 .. high)

    // Temporary (auxiliary) arrays -> this is the O(n) extra space.
    int* L = new int[n1];
    int* R = new int[n2];

    for (int i = 0; i < n1; i++) L[i] = arr[low + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = low;     // i -> L, j -> R, k -> write position in arr

    // Two-pointer merge. "<=" takes from the LEFT on ties, which keeps equal
    // elements in their original order => the sort is STABLE.
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else              arr[k++] = R[j++];
    }
    // One side is exhausted; copy whatever is left of the other side.
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;   // free the temporary arrays
    delete[] R;

    g_mergeCalls++;
    if (g_trace) {
        cout << "  merge [" << low << ".." << high << "] (mid=" << mid << ")  ->  ";
        printArray(g_traceArr, g_traceN);
        cout << "\n";
    }
}

// ----------------------------------------------------------------------------
// mergeSort: sorts arr[low..high] (inclusive) recursively.
// ----------------------------------------------------------------------------
void mergeSort(int arr[], int low, int high) {
    if (low >= high) return;               // 0 or 1 element: already sorted

    int mid = low + (high - low) / 2;      // overflow-safe midpoint
    mergeSort(arr, low, mid);              // sort left half
    mergeSort(arr, mid + 1, high);         // sort right half
    merge(arr, low, mid, high);            // combine the two sorted halves
}

// Helper: run one labelled test case on a COPY of the input.
void runTest(const char* title, const int input[], int n) {
    cout << "\n=== " << title << " ===\n";
    int* a = (n > 0) ? new int[n] : nullptr;
    for (int i = 0; i < n; i++) a[i] = input[i];

    cout << "Before: "; printArray(a, n); cout << "\n";
    g_traceArr = a;
    g_traceN = n;
    g_mergeCalls = 0;
    mergeSort(a, 0, n - 1);               // n = 0 -> high = -1 -> base case
    cout << "After : "; printArray(a, n); cout << "\n";
    cout << "merge() calls: " << g_mergeCalls << "\n";

    delete[] a;                            // safe even when a == nullptr
}

// Stability test: sort records by KEY only, using the same <= merge idea,
// and show equal keys keep their original order. (Small separate version of
// the algorithm that compares keys but moves whole records.)
struct Rec { int key; char tag; };

void mergeRec(Rec arr[], int low, int mid, int high) {
    int n1 = mid - low + 1, n2 = high - mid;
    Rec* L = new Rec[n1];
    Rec* R = new Rec[n2];
    for (int i = 0; i < n1; i++) L[i] = arr[low + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
    int i = 0, j = 0, k = low;
    while (i < n1 && j < n2) {
        if (L[i].key <= R[j].key) arr[k++] = L[i++];   // <= => stable
        else                      arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    delete[] L;
    delete[] R;
}
void mergeSortRec(Rec arr[], int low, int high) {
    if (low >= high) return;
    int mid = low + (high - low) / 2;
    mergeSortRec(arr, low, mid);
    mergeSortRec(arr, mid + 1, high);
    mergeRec(arr, low, mid, high);
}

int main() {
    // ---- Main trace required by the lab ----
    int main7[] = { 35, 12, 43, 8, 51, 27, 19 };
    runTest("Main trace: {35, 12, 43, 8, 51, 27, 19}", main7, 7);

    // ---- Boundary cases ----
    int sorted5[] = { 1, 2, 4, 5, 8 };
    int reverse5[] = { 8, 5, 4, 2, 1 };
    int repeated5[] = { 5, 2, 5, 1, 2 };
    int single1[] = { 42 };

    runTest("Already sorted {1, 2, 4, 5, 8}", sorted5, 5);
    runTest("Reverse sorted {8, 5, 4, 2, 1}", reverse5, 5);
    runTest("Repeated values {5, 2, 5, 1, 2}", repeated5, 5);
    runTest("Single element (n = 1)", single1, 1);
    runTest("Empty array (n = 0)", nullptr, 0);

    // ---- Stability demonstration ----
    cout << "\n=== Stability check: records (key,tag) sorted by key ===\n";
    Rec recs[] = { {5,'a'}, {2,'b'}, {5,'c'}, {1,'d'}, {2,'e'} };
    cout << "Before: ";
    for (int i = 0; i < 5; i++) cout << "(" << recs[i].key << "," << recs[i].tag << ") ";
    mergeSortRec(recs, 0, 4);
    cout << "\nAfter : ";
    for (int i = 0; i < 5; i++) cout << "(" << recs[i].key << "," << recs[i].tag << ") ";
    cout << "\nEqual keys keep original order: (2,b) before (2,e) and (5,a) before (5,c).\n";

    return 0;
}
