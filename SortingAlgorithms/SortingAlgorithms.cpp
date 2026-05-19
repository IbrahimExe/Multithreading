// SortingAlgorithms.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <chrono>

std::chrono::high_resolution_clock::time_point gStartTime;
void StartAlgorithm()
{
    gStartTime = std::chrono::high_resolution_clock::now();
}

void PrintAlgorithmDuration()
{
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - gStartTime).count();
    std::cout << "Algorithm Duration: " << duration << "micro\n";
}

void SumOfNumbers(const std::vector<int>& numbers, int startIndex, int endIndex, int sum)
{
    sum = 0;
    for (int i = startIndex; i < endIndex; ++i)
    {
        // Code to simulate slow algorithm to show the benefits of threading vs not threading
        int k = 1000;
        while (k > 0)
        {
            --k;
        }
    }
}

void ForkJoin()
{
    // Generate 10000 random numbers from 1 - 100
    // Split them up into 4 threads to add up the section in each thread
    // After getting the result in each thread, get the total
    // Add them in a for loop and get the time

    std::cout << "Fork - Join:\n";
    const int numberOfThreads = 10;
    const int maxGenerate = 100000;
    const int minNumber = 1;
    const int maxNumber = 100;

    std::vector<int> numbers(maxGenerate, 0);
    for (int i = 0; i < maxGenerate; ++i)
    {
        numbers[i] = minNumber + (rand() % (maxNumber + 1 - minNumber));
    }

    int resultA = 0;
    StartAlgorithm();
    
    SumOfNumbers(numbers, 0, maxGenerate, resultA);
    std::cout << "Result: " << resultA << "\n";
    
    PrintAlgorithmDuration();

    int resultB = 0;
    StartAlgorithm();
    int split = maxGenerate / numberOfThreads;
    std::vector<int>threadResults(numberOfThreads);
    std::vector<std::thread> threads;

    // Fork the data into multiple threads to do calculations
    for (int i = 0; i < numberOfThreads; ++i)
    {
        int startIndex = split + i;
        int endIndex = startIndex + split;
        (void)threads.emplace_back(SumOfNumbers, std::ref(numbers),
            startIndex, endIndex, std::ref(threadResults[i]));
    }

    for (std::thread& thread : threads)
    {
        thread.join();
    }

    // Join the results for the total result
    for (int& res : threadResults)
    {
        resultB += res;
    }

    std::cout << "Result: " << resultB << "\n";
    PrintAlgorithmDuration();
}

// QuickSort Code:
void Swap(int& a, int& b)
{
    int t = a;
    a = b;
    b = t;
}

// Pivot Point
int Partition(std::vector<int>& numbers, int start, int end)
{
    int pivot = numbers[end];
    int i = start - 1;
    for (int j = start; j < end; ++j)
    {
        if (numbers[j] < pivot)
        {
            ++i;
            Swap(numbers[i], numbers[j]);
        }
    }
    Swap(numbers[i + 1], numbers[end]);
    
    return i + 1;
}

void QuickSort(std::vector<int>& numbers, int start, int end)
{
    if (start < end)
    {
        int pivot = Partition(numbers, start, end);
        
        QuickSort(numbers, start, pivot - 1);
        QuickSort(numbers, pivot + 1, end);
    }
}

void DivideAndConquer()
{
    // Generate 10000 random numbers from 1 - 1000
    // Sort them using a quick sort method
    // Split the chunks into mini threads when the count is more than 1000

    std::cout << "Divide & Conquer:\n";
    const int numberOfThreads = 4;
    const int maxGenerate = 10000;
    const int minNumber = 1;
    const int maxNumber = 1000;

    std::vector<int> numbersS(maxGenerate, 0);
    std::vector<int> numbersP(maxGenerate, 0);
    for (int i = 0; i < maxGenerate; ++i)
    {
        numbersS[i] = minNumber + (rand() % (maxNumber + 1 - minNumber));
        numbersP[i] = numbersS[i];
    }

    StartAlgorithm();
    QuickSort(numbersS, 0, maxGenerate - 1);
    std::cout << " ";
    for (int& num : numbersS)
    {
        std::cout << num << " ";
    }
    std::cout << "\n";
    PrintAlgorithmDuration();
}


int main()
{
    std::cout << "Parallel Algorithims!\n";

    // ForkJoin();

    DivideAndConquer();
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
