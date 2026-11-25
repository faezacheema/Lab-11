#include <iostream>
using namespace std;

// ---- Globals used for counting / tracing only ----
static bool g_trace = true;     // print pivot, pi and array after each partition
static long g_comparisons = 0;        // number of arr[j] <= pivot comparisons
static int  g_depth = 0;        // current recursion depth
static int  g_maxDepth = 0;        // deepest recursion reached
static int* g_traceArr = nullptr;  // array being sorted (to print it whole)
static int  g_traceN = 0;

void printArray(const int arr[], int n) {
    cout << "{";
    for (int i = 0; i < n; i++) {
        cout << arr[i];
        if (i + 1 < n) cout << ", ";
    }
    cout << "}";
}

void swapInts(int& a, int& b) { int t = a; a = b; b = t; }

// ----------------------------------------------------------------------------
// partition (Lomuto): pivot = arr[high].
// After it returns pi:   arr[low..pi-1] <= pivot,  arr[pi] == pivot (final
// position),  arr[pi+1..high] > pivot.
// ----------------------------------------------------------------------------
int partition(int arr[], int low, int high) {
    int pivot = arr[high];     // last element is the pivot
    int i = low - 1;           // end of the "<= pivot" region (initially empty)

    for (int j = low; j < high; j++) {      // stop before high (the pivot)
        g_comparisons++;                    // one element comparison
        if (arr[j] <= pivot) {
            i++;
            swapInts(arr[i], arr[j]);       // move arr[j] into the left region
        }
        // else: arr[j] > pivot stays in the right region
    }
    swapInts(arr[i + 1], arr[high]);        // drop pivot between the two regions
    int pi = i + 1;

    if (g_trace) {
        cout << "  partition [" << low << ".." << high << "]  pivot = " << pivot
            << ", pi = " << pi << "  ->  ";
        printArray(g_traceArr, g_traceN);
        cout << "\n";
    }
    return pi;
}

// ----------------------------------------------------------------------------
// quickSort: sorts arr[low..high] (inclusive).
// ----------------------------------------------------------------------------
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        g_depth++;
        if (g_depth > g_maxDepth) g_maxDepth = g_depth;

        int pi = partition(arr, low, high);   // pivot lands at its final index
        quickSort(arr, low, pi - 1);          // left of pivot  (pi excluded)
        quickSort(arr, pi + 1, high);         // right of pivot (pi excluded)

        g_depth--;
    }
}

// ---- Test helpers ----
// Trace test: prints every partition step.
void traceTest(const char* title, const int input[], int n) {
    cout << "\n=== " << title << " ===\n";
    int* a = (n > 0) ? new int[n] : nullptr;
    for (int i = 0; i < n; i++) a[i] = input[i];

    cout << "Before: "; printArray(a, n); cout << "\n";
    g_traceArr = a; g_traceN = n;
    g_comparisons = 0; g_depth = 0; g_maxDepth = 0;
    g_trace = true;
    quickSort(a, 0, n - 1);
    cout << "After : "; printArray(a, n); cout << "\n";
    cout << "Comparisons = " << g_comparisons << ", max recursion depth = " << g_maxDepth << "\n";
    delete[] a;
}

// Counting test: silent (no partition trace), prints one summary line.
void countTest(const char* label, const int input[], int n) {
    int* a = (n > 0) ? new int[n] : nullptr;
    for (int i = 0; i < n; i++) a[i] = input[i];

    g_traceArr = a; g_traceN = n;
    g_comparisons = 0; g_depth = 0; g_maxDepth = 0;
    g_trace = false;
    quickSort(a, 0, n - 1);

    cout << label << "\n   input -> ";
    printArray(input, n);
    cout << "\n   sorted-> "; printArray(a, n);
    cout << "\n   comparisons = " << g_comparisons
        << ", max recursion depth = " << g_maxDepth << "\n";
    delete[] a;
}

int main() {
    // ---- Traces required by the lab ----
    int t1[] = { 2, 1, 5, 4, 6, 3 };
    int t2[] = { 35, 12, 43, 8, 51, 27, 19 };
    traceTest("Trace 1: {2, 1, 5, 4, 6, 3}", t1, 6);
    traceTest("Trace 2: {35, 12, 43, 8, 51, 27, 19}", t2, 7);

    // ---- Comparison counts ----
    cout << "\n=== Comparison counts (arr[j] <= pivot) ===\n";
    int random5[] = { 5, 1, 4, 2, 8 };
    int sorted5[] = { 1, 2, 4, 5, 8 };
    int reverse5[] = { 8, 5, 4, 2, 1 };
    int repeated5[] = { 7, 7, 7, 7, 7 };
    int single1[] = { 9 };

    countTest("Random order", random5, 5);
    countTest("Already sorted", sorted5, 5);
    countTest("Reverse sorted", reverse5, 5);
    countTest("Repeated values", repeated5, 5);
    countTest("Single element (n = 1)", single1, 1);
    countTest("Empty array (n = 0)", nullptr, 0);

    // ---- Worst case grows quadratically: n(n-1)/2 on sorted input ----
    cout << "\n=== Growth on already-sorted input (last-element pivot) ===\n";
    cout << "   n    comparisons   n(n-1)/2   max depth\n";
    int sizes[] = { 5, 10, 50, 100, 500 };
    for (int s = 0; s < 5; s++) {
        int n = sizes[s];
        int* a = new int[n];
        for (int i = 0; i < n; i++) a[i] = i + 1;
        g_traceArr = a; g_traceN = n;
        g_comparisons = 0; g_depth = 0; g_maxDepth = 0;
        g_trace = false;
        quickSort(a, 0, n - 1);
        cout << "  " << n << "\t" << g_comparisons << "\t\t" << (long)n * (n - 1) / 2
            << "\t   " << g_maxDepth << "\n";
        delete[] a;
    }
    return 0;
}
