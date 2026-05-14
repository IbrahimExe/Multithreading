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


int main()
{
    std::cout << "Hello World!\n";
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
