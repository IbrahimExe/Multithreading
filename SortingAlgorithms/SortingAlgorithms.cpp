// SortingAlgorithms.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <chrono>
#include <queue>
#include <iomanip> // For std::setprecision

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
        // Slowing it down manually
        /*int k = 1000;
        while (k > 0)
        {
            --k;
        }*/

        int pivot = Partition(numbers, start, end);
        
        QuickSort(numbers, start, pivot - 1);
        QuickSort(numbers, pivot + 1, end);
    }
}

void ParallelQuickSort(std::vector<int>& numbers, int start, int end, int maxParallelSize)
{
    if (start < end)
    {
        // Slowing it down manually
        /*int k = 1000;
        while (k > 0)
        {
            --k;
        }*/

        int pivot = Partition(numbers, start, end);
        if (end - start > maxParallelSize)
        {
            std::thread leftThread(ParallelQuickSort, std::ref(numbers), start, pivot - 1, maxParallelSize);
            std::thread rightThread(ParallelQuickSort, std::ref(numbers), pivot + 1, end, maxParallelSize);
            rightThread.join();
            leftThread.join();
        }
        else
        {
            ParallelQuickSort(numbers, start, pivot - 1, maxParallelSize);
            ParallelQuickSort(numbers, pivot + 1, end, maxParallelSize);
        }
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

    // Quick Sort
    StartAlgorithm();
    QuickSort(numbersS, 0, maxGenerate - 1);
    std::cout << " ";
    for (int& num : numbersS)
    {
        std::cout << num << " ";
    }
    std::cout << "\n";
    PrintAlgorithmDuration();

    // Parallel Quick Sort
    StartAlgorithm();
    int maxParallelSize = 1000;
    ParallelQuickSort(numbersS, 0, maxGenerate - 1, maxParallelSize);
    std::cout << " ";
    for (int& num : numbersS)
    {
        std::cout << num << " ";
    }
    std::cout << "\n";
    PrintAlgorithmDuration();
}

class NumberBuffer
{
public:
    NumberBuffer(int capacity)
        : mCapacity(capacity)
    {

    }

    void Push(float number)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mNotFull.wait(lock, [this]() {return mNumbers.size() < mCapacity; });

        mNumbers.push(number);

        mNotEmpty.notify_one();
    }

    float Pop()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mNotEmpty.wait(lock, [this]() {return !mNumbers.empty(); });

        float number = mNumbers.front();
        mNumbers.pop();

        mNotFull.notify_one();
        return number;
    }

private:
    std::queue<float> mNumbers;
    int mCapacity = 0;
    std::mutex mMutex;
    std::condition_variable mNotFull;
    std::condition_variable mNotEmpty;
};

void GenerateNumbers(NumberBuffer& numbers, int maxNumbers)
{
    for (int i = 0; i < maxNumbers; ++i)
    {
        float number = 1.0f + (1001.0f * (static_cast<float>(rand()) / RAND_MAX));
        numbers.Push(number);
    }
}

void GetMinMax(NumberBuffer& numbers, float& min, float& max)
{
    min = FLT_MAX;
    max = FLT_MIN;


}

void ProducerConsumer()
{
    // Have a producer thread generate 10000 random float numbers
    // Have 4 consumer threads obtain the numbers as they are available
    // Consumers should store the min and the max numbers generated
    // Print the min / max in each thread
    // Once all complete, print the overall min / max values

    std::cout << "Producer - Consumer:\n";
    const int numberOfThreads = 4;
    const int maxGenerate = 10000;
    const int minNumber = 1;
    const int maxNumber = 1000;

    NumberBuffer numbers(100); // Buffer capacity of 100
    std::vector<float>threadMins(numberOfThreads);
    std::vector<float>threadMaxs(numberOfThreads);

    std::thread producer(GenerateNumbers, std::ref(numbers), maxGenerate);
    std::vector<std::thread> consumers;
    for (int i = 0; i < numberOfThreads; ++i)
    {
        (void)consumers.emplace_back(GetMinMax, std::ref(numbers), std::ref(threadMins[i]), std::ref(threadMaxs[i]));
    }

    for (std::thread& consumer : consumers)
    {
        consumer.join();
    }
    producer.join();
    float totalMin = FLT_MAX;
    float totalMax = FLT_MIN;
    for (int i = 0; i < numberOfThreads; ++i)
    {
        totalMin = std::min(totalMin, threadMins[i]);
        totalMax = std::max(totalMax, threadMaxs[i]);
    }

    std::cout << std::setprecision(5) << "Min: " << totalMin << " - Max: " << totalMax << "\n";
}

class SafeQueue
{
public:
    void Push(int value)
    {
        {
            std::unique_lock<std::mutex> lk(mMutex);
            mNumbers.push(value);
        }
        mNotEmpty.notify_one();
    }
    int Pop()
    {
        std::unique_lock<std::mutex> lk(mMutex);
        mNotEmpty.wait(lk, [this]() { return !mNumbers.empty() || mFinished; });

        // assume everything is positive
        if (mNumbers.empty())
        {
            return -1;
        }

        int value = mNumbers.front();
        mNumbers.pop();
        return value;
    }
    void SetFinished()
    {
        {
            std::lock_guard<std::mutex> lk(mMutex);
            mFinished = true;
        }
        mNotEmpty.notify_all();
    }
private:
    std::queue<int> mNumbers;
    std::mutex mMutex;
    std::condition_variable mNotEmpty;
    bool mFinished = false;
};

void Part1GenerateNumbers(SafeQueue& numbers, int maxNumbers)
{
    for (int i = 0; i < maxNumbers; ++i)
    {
        int number = 1 + (rand() % 10);
        numbers.Push(number);
    }
    numbers.SetFinished();
    std::cout << "Stage 1: Complete\n";
}
void Part2SumUpNumberIndices(SafeQueue& numbers, SafeQueue& processedNumbers)
{
    while (true)
    {
        int number = numbers.Pop();
        if (number < 0)
        {
            break;
        }
        int total = 0;
        for (int i = 1; i < number; ++i)
        {
            total += i;
        }
        processedNumbers.Push(total);
    }
    processedNumbers.SetFinished();
    std::cout << "Stage 2: Complete\n";
}
void Part3PrintResults(SafeQueue& processedNumbers)
{
    while (true)
    {
        int number = processedNumbers.Pop();
        if (number < 0)
        {
            break;
        }
        std::cout << "Processed: " << number << "\n";
    }

    std::cout << "Stage 3: Complete\n";
}

void Pipeline()
{
    // Excercise 4
    // Create three functions​
    // One generates a random 100 int numbers from 1 - 10​
    // One takes those numbers and adds sums up the numbers in the value​
    // (eg: if first one is 3, add 1 + 2 + 3)​
    // One prints the results from the second one
    std::cout << "Pipeline:\n";
    SafeQueue numbers;
    SafeQueue processedNumbers;
    std::thread stage1(Part1GenerateNumbers, std::ref(numbers), 1000);
    std::thread stage2(Part2SumUpNumberIndices, std::ref(numbers), std::ref(processedNumbers));
    std::thread stage3(Part3PrintResults, std::ref(processedNumbers));

    stage3.join();
    stage2.join();
    stage1.join();
}

int main()
{
    std::cout << "Parallel Algorithims!\n";

    // ForkJoin();

    // DivideAndConquer();

    // ProducerConsumer();

    Pipeline();
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
