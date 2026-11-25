# REFLECTION REPORT: AUCTIONHUB SORTING ENGINE

**Course:** Data Structures & Algorithms  
**File:** REFLECTION.md

---

## 1. Real-World Justification for Each Algorithm

In the AuctionHub system, different modules handle different types of bid data. Therefore, using the same sorting algorithm for every task would not always be the best choice. Each algorithm has its own advantages depending on the amount of data and the purpose of the module.

- **Insertion Sort (Module A – Live Auction Monitor):** New bids are usually added to an existing sequence that is already nearly sorted. Insertion Sort is suitable for this situation because it performs well on nearly sorted data, with a best-case time complexity of O(n). It is also simple to implement and does not require recursive calls.

- **Selection Sort (Module B – Top-K Bid Finder):** This module focuses on finding the top three bidders. Selection Sort is useful for small datasets because it repeatedly selects the largest remaining element and moves it to the correct position. It also performs relatively few swaps and does not require additional array memory.

- **Interchange Sort (Module C – Price Anomaly Validator):** This module checks small batches of records before they are stored in the database. Since each batch contains at most 50 records, the O(n²) time complexity is acceptable for this situation. Its simple comparison structure also makes the code easy to understand and verify.

- **Bubble Sort with Early Stop (Module D – Stabilization Detector):** This module checks whether a sequence of bids has become stable. By using a `swapped` flag, Bubble Sort can stop early when no elements need to be exchanged. In this case, it only requires O(n) time, making it useful for checking an already sorted sequence.

- **Quick Sort (Module E – Full Historical Sorter):** This module handles large amounts of historical bidding data for end-of-day reports. Quick Sort is a suitable choice because its average-case time complexity is O(n log n). It also sorts the data in-place, avoiding the need for another large array.

## 2. Quick Sort Benchmark on 100,000 Records

To evaluate the performance of Quick Sort, I generated 100,000 random Bid records using `std::mt19937`. I then measured the sorting time using `std::chrono::high_resolution_clock`.

The experiment showed that sorting all 100,000 records took approximately **22.301 ms (0.022 seconds)** on my machine. After sorting, I checked the result to make sure the records were arranged in ascending order.

This experiment demonstrates that Quick Sort can handle a relatively large dataset efficiently. However, the actual execution time may vary depending on the computer and system environment.

## 3. Debugging Experience: One Bug I Encountered

During testing, I encountered a segmentation fault when Quick Sort received an empty vector.

After investigating the problem, I found that `v.size()` returns an unsigned `size_t` value in C++. When the vector is empty, calculating `v.size() - 1` causes an underflow, producing a very large positive number instead of a negative index.

To solve this problem, I added a condition at the beginning of `quickSortWrapper()`:

`if (v.size() <= 1) return;`

This prevents the function from attempting to access invalid array positions when the vector contains zero or one element.

Through this debugging process, I learned that handling edge cases is important, especially when working with arrays, vectors, and recursive algorithms.

## 4. LLM Usage Log (Green / Yellow / Red Policy)

I used ChatGPT as a supporting tool during development, mainly to clarify algorithm concepts, understand complexity, and review specific programming issues. The following are examples of how I used AI:

- **Prompt 1 (Green Zone):** "Explain why insertion sort is O(n) on nearly-sorted input."  
  Used to better understand the performance of Insertion Sort when processing nearly sorted data.

- **Prompt 2 (Green Zone):** "Generate C++ code using std::mt19937 to produce 100,000 random Bid structs and write them to a file."  
  Used to help prepare random data for the Quick Sort benchmark.

- **Prompt 3 (Yellow Zone):** "What's the difference between Lomuto and Hoare partition schemes, and how does median-of-three work with Lomuto?"  
  Used to understand different partitioning strategies and the reason for choosing median-of-three as the pivot selection method.

AI was used as a learning and supporting tool. I still needed to review the generated suggestions and understand the logic before applying them to my project.

## 5. Worst-Case Complexity of Quick Sort with Median-of-Three

Although median-of-three improves pivot selection, it does not guarantee that Quick Sort will always run in O(n log n) time.

This strategy selects the median value among the first, middle, and last elements of a partition. It helps avoid highly unbalanced partitions in common cases, such as already sorted or reverse-sorted arrays.

However, median-of-three only examines three positions. A specially arranged input can still cause it to repeatedly select a poor pivot, resulting in very unbalanced partitions.

In this situation, Quick Sort may still require O(n²) time in the worst case.

Therefore, median-of-three is an effective improvement for practical performance, but it does not eliminate the theoretical worst-case complexity of Quick Sort.
