// Parallel MergeSort Assignment
/*
For this assignment, use the "Merge Sort Challenge" file provided.

Attached is a cpp file for sequential MergeSort. There is also a shell for
parallel MergeSort.
- First use the methods we used for QuickSort to implement the shell method.
- Then use the optimization method we discussed in class, to avoid splitting
  the array all the way to 1 or 0 size array. Choose a fixed side L for 
  lowest size array and then use std::sort to sort the array directly
  instead of continuing the splitting and recursion.
- The file averages sequential and parallel sort methods for 10,000,000 random
  integers and print the result.
- Add QuickSOrt to this file so we can compare MergeSort with QuickSort parallel
  solutions. Add proper print out for this.
- Add printing for the L value you found that gives the most efficient parallelization.
  Make sure you sue L for sequential and parallel functions.
- As usual hand in cpp and exe in release mode.
*/

// Merge Sort Challenge.txt file: 
// Challenge: Sort an array of random integers with merge sort

#include <thread>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm> // for std::sort
#include <omp.h> // OpenMP

using namespace std;

// L value: The threshold at which we stop parallelizing/ recursing and just use std::sort 
const unsigned int L = 10000;

// Max Depth prevents OpenMP from creating too many threads  when the array is large.
const int MAX_DEPTH = 4;

/* declaration of merge helper function */
void merge(int* array, unsigned int left, unsigned int mid, unsigned int right);

void sequential_quick_sort(int* array, int left, int right);

/* sequential implementation of merge sort */
void sequential_merge_sort(int* array, unsigned int left, unsigned int right)
{
	if (right - left <= L)
	{
		std::sort(array + left, array + right + 1);
		return;
	}

	if (left < right)
	{
		unsigned int mid = (left + right) / 2; // find the middle point
		sequential_merge_sort(array, left, mid); // sort the left half
		sequential_merge_sort(array, mid + 1, right); // sort the right half
		merge(array, left, mid, right); // merge the two sorted halves
	}
}

void parallel_merge_sort_helper(int* array, unsigned int left, unsigned int right, int depth)
{
	// If subarray size is <= L, use std::sort
	if (right - left <= L)
	{
		std::sort(array + left, array + right + 1);
		return;
	}

	if (left < right)
	{
		unsigned int mid = left + (right - left) / 2;

		// Limit number of threads using MAX_DEPTH
		if (depth < MAX_DEPTH)
		{
			#pragma omp parallel sections
			{
				#pragma omp section
				{
					parallel_merge_sort_helper(array, left, mid, depth + 1);
				}
				
				#pragma omp section
				{
					parallel_merge_sort_helper(array, mid + 1, right, depth + 1);
				}
			}
		}
		else
		{
			// Fall back to sequential if we have reached our max concurrent thread depth
			sequential_merge_sort(array, left, mid);
			sequential_merge_sort(array, mid + 1, right);
		}
		merge(array, left, mid, right); // merge the two sorted halves
	}
}

/* parallel implementation of merge sort */
void parallel_merge_sort(int* array, unsigned int left, unsigned int right)
{
	omp_set_nested(1); // explicitly allow nested parallelism in OpenMP
	parallel_merge_sort_helper(array, left, right, 0);
}

/* helper function to merge two sorted subarrays
   array[l..m] and array[m+1..r] into array */
void merge(int* arr, unsigned int left, unsigned int mid, unsigned int right)
{
	unsigned int num_left = mid - left + 1; // number of elements in left subarray
	unsigned int num_right = right - mid; // number of elements in right subarray

	// copy data into temporary left and right subarrays to be merged
	//auto  array_left{ std::make_shared<int[]>(num_left) };
	//auto  array_right{ std::make_shared<int[]>(num_right) };

	int* array_left = new int[num_left];
	int* array_right = new int[num_right];

	std::copy(&arr[left], &arr[mid + 1], array_left);
	std::copy(&arr[mid + 1], &arr[right + 1], array_right);

	// initialize indices for array_left, array_right, and input subarrays
	unsigned int index_left = 0;    // index to get elements from array_left
	unsigned int index_right = 0;    // index to get elements from array_right
	unsigned int index_insert = left; // index to insert elements into input array

	// merge temporary subarrays into original input array
	while ((index_left < num_left) || (index_right < num_right))
	{
		if ((index_left < num_left) && (index_right < num_right))
		{
			if (array_left[index_left] <= array_right[index_right])
			{
				arr[index_insert] = array_left[index_left];
				index_left++;
			}
			else
			{
				arr[index_insert] = array_right[index_right];
				index_right++;
			}
		}
		// copy any remain elements of array_left into array
		else if (index_left < num_left)
		{
			arr[index_insert] = array_left[index_left];
			index_left += 1;
		}
		// copy any remain elements of array_right into array
		else if (index_right < num_right)
		{
			arr[index_insert] = array_right[index_right];
			index_right += 1;
		}
		++index_insert;
	}

	delete[] array_left;
	delete[] array_right;
}

// Quick Sort Implementation

// Sequential implementation of quick sort
void sequential_quick_sort(int* array, int left, int right)
{
	if (right - left <= L)
	{
		std::sort(array + left, array + right + 1);
		return;
	}

	int l = left, r = right;
	int pivot = array[left + (right - left) / 2];

	while (l <= r)
	{
		while (array[l] < pivot)
		{
			l++;
		}
		while (array[r] > pivot)
		{
			r--;
		}
		if (l <= r)
		{
			std::swap(array[l], array[r]);
			l++;
			r--;
		}
	}

	if (left < r)
	{
		sequential_quick_sort(array, left, r);
	}
	
	if (l < right)
	{
		sequential_quick_sort(array, l, right);
	}
}

// helper function for parallel Quick Sort with depth tracking
void parallel_quick_sort_helper(int* array, int left, int right, int depth)
{
	if (right - left <= L)
	{
		std::sort(array + left, array + right + 1);
		return;
	}

	int l = left, r = right;
	int pivot = array[left + (right - left) / 2];

	while (l <= r)
	{
        while (array[l] < pivot)
        {
            l++;
        }
        while (array[r] > pivot)
        {
            r--;
        }
        if (l <= r)
        {
            std::swap(array[l], array[r]);
            l++;
            r--;
        }
	}

	if (depth < MAX_DEPTH)
	{
		#pragma omp parallel sections
		{
			#pragma omp section 
			{
				if (left < r)
				{
					parallel_quick_sort_helper(array, left, r, depth + 1);
				}
			}
			#pragma omp section
			{
				if (l < right)
				{
					parallel_quick_sort_helper(array, l, right, depth + 1);
				}
			}
		}
	}
	else
	{
		if (left < r)
		{
			sequential_quick_sort(array, left, r);
		}
		if (l < right)
		{
			sequential_quick_sort(array, l, right);
		}
	}
}

// Parallel implementation of Quick Sort
void parallel_quick_sort(int* array, int left, int right)
{
	omp_set_nested(1);
	parallel_quick_sort_helper(array, left, right, 0);
}

int main()
{
	mt19937 generator((unsigned int)chrono::system_clock::now().time_since_epoch().count());

	const int NUM_EVAL_RUNS = 2;
	const int N = 10000000; // number of elements to sort

	cout << "L (Threshold) Value: " << L << "\n";

	std::vector<int> original_array, sequential_result, parallel_result;
	std::vector<int> sequential_qs_result, parallel_qs_result;

	original_array.reserve(N);
	sequential_result.reserve(N);
	parallel_result.reserve(N);
	sequential_qs_result.reserve(N);
    parallel_qs_result.reserve(N);

	for (int i = 0; i < N; i++)
	{
		original_array.push_back(generator() % 2000000 - 1000000);
	}

	// Merge SOrt Evaluation
	cout << "Evaluating Sequential Merge Sort...\n";
	std::chrono::duration<double> sequential_time(0);
	sequential_result = original_array;
	sequential_merge_sort(&sequential_result[0], 0, N - 1); // 'warm up'

	for (int i = 0; i < NUM_EVAL_RUNS; i++)
	{
		sequential_result = original_array; // reset result array
		auto start_time = std::chrono::high_resolution_clock::now();
		sequential_merge_sort(&sequential_result[0], 0, N - 1);
		sequential_time += std::chrono::high_resolution_clock::now() - start_time;
	}
	sequential_time /= NUM_EVAL_RUNS;

	//cout << "Evaluating Sequential Implementation...\n";
	//std::chrono::duration<double> sequential_time(0);
	//sequential_result = original_array;
	////std::copy(&original_array[0], &original_array[N-1], sequential_result);
	//sequential_merge_sort(&sequential_result[0], 0, N - 1); // "warm up"	
	//for (int i = 0; i < NUM_EVAL_RUNS; i++)
	//{
	//	sequential_result = original_array;
	//	//	std::copy(&original_array[0], &original_array[N-1], sequential_result); // reset result array
	//	auto start_time = std::chrono::high_resolution_clock::now();
	//	sequential_merge_sort(&sequential_result[0], 0, N - 1);
	//	sequential_time += std::chrono::high_resolution_clock::now() - start_time;
	//}
	//sequential_time /= NUM_EVAL_RUNS;

	printf("Evaluating Parallel Merge Sort...\n");
	std::chrono::duration<double> parallel_time(0);
	parallel_result = original_array;
	//	std::copy(&original_array[0], &original_array[N-1], parallel_result);
	parallel_merge_sort(&parallel_result[0], 0, N - 1); // "warm up"
	
	for (int i = 0; i < NUM_EVAL_RUNS; i++)
	{
		parallel_result = original_array;
		//	std::copy(&original_array[0], &original_array[N - 1], parallel_result); // reset result array
		auto start_time = std::chrono::high_resolution_clock::now();
		parallel_merge_sort(&parallel_result[0], 0, N - 1);
		parallel_time += std::chrono::high_resolution_clock::now() - start_time;
	}
	parallel_time /= NUM_EVAL_RUNS;

	//printf("Evaluating Parallel Implementation...\n");
	//std::chrono::duration<double> parallel_time(0);

	//parallel_result = original_array;
	////	std::copy(&original_array[0], &original_array[N-1], parallel_result);
	//parallel_merge_sort(&parallel_result[0], 0, N - 1); // "warm up"
	//for (int i = 0; i < NUM_EVAL_RUNS; i++)
	//{
	//	parallel_result = original_array;
	//	//	std::copy(&original_array[0], &original_array[N - 1], parallel_result); // reset result array
	//	auto start_time = std::chrono::high_resolution_clock::now();
	//	parallel_merge_sort(&parallel_result[0], 0, N - 1);
	//	parallel_time += std::chrono::high_resolution_clock::now() - start_time;
	//}
	//parallel_time /= NUM_EVAL_RUNS;

	// verify sequential and parallel results are same
	for (int i = 0; i < 10; i++)
	{
		if (sequential_result[i] != parallel_result[i])
		{
			cout << "ERROR: Merge Sort result mismatch at index! " << i << endl;
		}
	}

	// Quick Sort Evaluations
	cout << "\nEvaluating Sequential Quick Sort...\n";
	std::chrono::duration<double> sequential_qs_time(0);
	sequential_qs_result = original_array;
	sequential_quick_sort(&sequential_qs_result[0], 0, N - 1);

	for (int i = 0; i < NUM_EVAL_RUNS; i++)
	{
		sequential_qs_result = original_array;
		auto start_time = std::chrono::high_resolution_clock::now();
		sequential_quick_sort(&sequential_qs_result[0], 0, N - 1);
		sequential_qs_time += std::chrono::high_resolution_clock::now() - start_time;
	}
	sequential_qs_time /= NUM_EVAL_RUNS;

	
	cout << "Evaluating Parallel Quick Sort...\n";
	std::chrono::duration<double> parallel_qs_time(0);
	parallel_qs_result = original_array;
	parallel_quick_sort(&parallel_qs_result[0], 0, N - 1);

	for (int i = 0; i < NUM_EVAL_RUNS; i++)
	{
		parallel_qs_result = original_array;
		auto start_time = std::chrono::high_resolution_clock::now();
		parallel_quick_sort(&parallel_qs_result[0], 0, N - 1);
		parallel_qs_time += std::chrono::high_resolution_clock::now() - start_time;
	}
	parallel_qs_time /= NUM_EVAL_RUNS;

	// Verify seq and parallel quick sort results are teh same
	for (int i = 0; i < 10; i++)
	{
		if (sequential_qs_result[i] != parallel_qs_result[i])
		{
			cout << "ERROR: Quick Sort results mismatch at index! " << i << endl;
		}
	}


	// Final Results Messages Output:
	cout << "\n   Final Results:   \n";
	cout << "MERGE SORT: \n";
	cout << "Average Sequential Time: " << sequential_time.count() * 1000 << " ms\n";
	cout << "Average Parallel Time: " << parallel_time.count() * 1000 << " ms \n";
	cout << "Speedup: " << sequential_time / parallel_time << "x \n";
	cout << "Efficiency: " << 100 * (sequential_time / parallel_time) / std::thread::hardware_concurrency() << "% \n\n";

	cout << "QUICK SORT:\n";
	cout << "Average Sequential Time: " << sequential_qs_time.count() * 1000 << " ms \n";
	cout << "Average Parallel Time: " << parallel_qs_time.count() * 1000 << " ms \n";
	cout << "Speedup: " << sequential_qs_time / parallel_qs_time << "x \n";
	cout << "Efficiency: " << 100 * (sequential_qs_time / parallel_qs_time) / std::thread::hardware_concurrency() << "% \n";

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
