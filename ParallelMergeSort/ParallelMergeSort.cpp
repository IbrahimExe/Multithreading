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

using namespace std;

/* declaration of merge helper function */
void merge(int* array, unsigned int left, unsigned int mid, unsigned int right);

/* sequential implementation of merge sort */
void sequential_merge_sort(int* array, unsigned int left, unsigned int right)
{
	if (left < right)
	{
		unsigned int mid = (left + right) / 2; // find the middle point
		sequential_merge_sort(array, left, mid); // sort the left half
		sequential_merge_sort(array, mid + 1, right); // sort the right half
		merge(array, left, mid, right); // merge the two sorted halves
	}
}

/* parallel implementation of merge sort */
void parallel_merge_sort(int* array, unsigned int left, unsigned int right)
{
	/***********************
	 * YOUR CODE GOES HERE *
	 ***********************/
}

/* helper function to merge two sorted subarrays
   array[l..m] and array[m+1..r] into array */
void merge(int* arr, unsigned int left, unsigned int mid, unsigned int right)
{
	unsigned int num_left = mid - left + 1; // number of elements in left subarray
	unsigned int num_right = right - mid; // number of elements in right subarray

	// copy data into temporary left and right subarrays to be merged
	auto  array_left{ std::make_shared<int[]>(num_left) };
	auto  array_right{ std::make_shared<int[]>(num_right) };

	std::copy(&arr[left], &arr[mid + 1], array_left.get());
	std::copy(&arr[mid + 1], &arr[right + 1], array_right.get());

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
}

int main()
{
	mt19937 generator((unsigned int)chrono::system_clock::now().time_since_epoch().count());

	const int NUM_EVAL_RUNS = 2;
	const int N = 10000000; // number of elements to sort

	std::vector<int> original_array, sequential_result, parallel_result;
	original_array.reserve(N);
	sequential_result.reserve(N);
	parallel_result.reserve(N);

	for (int i = 0; i < N; i++)
	{
		original_array.push_back(generator() % 2000000 - 1000000);
	}

	cout << "Evaluating Sequential Implementation...\n";
	std::chrono::duration<double> sequential_time(0);
	sequential_result = original_array;
	//std::copy(&original_array[0], &original_array[N-1], sequential_result);
	sequential_merge_sort(&sequential_result[0], 0, N - 1); // "warm up"	
	for (int i = 0; i < NUM_EVAL_RUNS; i++)
	{
		sequential_result = original_array;
		//	std::copy(&original_array[0], &original_array[N-1], sequential_result); // reset result array
		auto start_time = std::chrono::high_resolution_clock::now();
		sequential_merge_sort(&sequential_result[0], 0, N - 1);
		sequential_time += std::chrono::high_resolution_clock::now() - start_time;
	}
	sequential_time /= NUM_EVAL_RUNS;

	printf("Evaluating Parallel Implementation...\n");
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

	// verify sequential and parallel results are same
	for (int i = 0; i < 10; i++)
	{
		if (sequential_result[i] != parallel_result[i])
		{
			cout << "ERROR: Result mismatch at index " << i << endl;
		}
	}
	cout << "Average Sequential Time: " << sequential_time.count() * 1000 << " ms \n";
	cout << "  Average Parallel Time: " << parallel_time.count() * 1000 << " ms \n";
	cout << "Speedup: " << sequential_time / parallel_time << " ms \n";
	cout << "Efficiency " << 100 * (sequential_time / parallel_time) / std::thread::hardware_concurrency() << " ms \n";

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
