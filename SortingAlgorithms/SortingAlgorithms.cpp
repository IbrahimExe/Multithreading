// SortingAlgorithms.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <chrono>
#include <queue>
#include <deque>
#include <iomanip> // For std::setprecision
#include <stack>
#include <algorithm> // for std::sort
#include <functional> // for std::function
#include <execution> 

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

std::mutex gPrintMutex;
// Queue for workers to Add/ Remove/ Steal from other workers
class WorkQueue
{
public:
    WorkQueue()
    {

    }

    WorkQueue(const WorkQueue& copy)
    {
        mTasks = copy.mTasks;
    }

    void Push(std::function<void()> task)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mTasks.push_front(task);
    }

    bool Pop(std::function<void()>& task)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        if (mTasks.empty())
        {
            return false;
        }
        task = std::move(mTasks.front());
        mTasks.pop_front();
        return true;
    }

    bool Empty()
    {
        std::lock_guard<std::mutex> lock(mMutex);
        return mTasks.empty();
    }

private:
    std::deque<std::function<void()>> mTasks;
    std::mutex mMutex;
};

// Task Queue to add tasks to take from
class WorkerTaskPool
{
public:
    WorkerTaskPool(int capacity)
    {
        mQueues.resize(capacity);
        for (int i = 0; i < capacity; ++i)
        {
            mWorkers.emplace_back(&WorkerTaskPool::WorkerThread, this, i);
        }
    }
    ~WorkerTaskPool()
    {
        FinishTasks();
    }

    void FinishTasks()
    {
        if (mDone)
        {
            return;
        }

        mDone = true;
        for (auto& workerThread : mWorkers)
        {
            if (workerThread.joinable())
            {
                workerThread.join();
            }
        }
    }

    void CreateTask(std::function<void()> task)
    {
        int index = rand() % mQueues.size();
        mQueues[index].Push(std::move(task));
    }

private:
    std::vector<std::thread> mWorkers;
    std::vector<WorkQueue> mQueues;
    std::atomic<bool> mDone;

    void WorkerThread(int index)
    {
        while (!mDone)
        {
            std::function<void()> task;
            if (mQueues[index].Pop(task))
            {
                task();
            }
            else
            {
                // Try to steal from another worker
                bool stolen = false;
                for (int i = 0; i < mQueues.size(); ++i)
                {
                    if (i != index && mQueues[i].Pop(task))
                    {
                        task();
                        stolen = true;
                        std::lock_guard<std::mutex> lock(gPrintMutex);
                        std::cout << "Task STOLEN from " << i << " by " << index << "\n";
                        break;
                    }
                }
                if (!stolen)
                {
                    // No work found, sleep
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                }
            }
        }
    }
};

void WorkerSteal()
{
    WorkerTaskPool taskPool(4);
    for (int i = 0; i < 40; ++i)
    {
        taskPool.CreateTask([i]() {
            int workTime = (rand() % 450) + 50;
            std::this_thread::sleep_for(std::chrono::milliseconds(workTime));
            std::lock_guard<std::mutex> lock(gPrintMutex);
            std::cout << "Task (" << i << ") completed by worker " << std::this_thread::get_id() << "\n";
            });
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));
    taskPool.FinishTasks();
    std::cout << "All Tasks Complete\n";
}

void MapChunk(const std::vector<std::string>& words, int start, int end, int& result, std::mutex& mtx)
{
    int sumLocal = 0;
    for (int i = start; i < end; ++i)
    {
        for (int c = 0; c < words[i].size(); ++c)
        {
            ++sumLocal;
        }
    }

    std::lock_guard<std::mutex> lock(mtx);
    result += sumLocal;
}

int WordLenghtSum(const std::vector<std::string>& words, int numOfThreads)
{
    int totalSum = 0;
    std::mutex mutex;
    std::vector<std::thread> threads;

    int chunkSize = (words.size() + numOfThreads - 1) / numOfThreads; // Even distribution of work among threads
    for (int i = 0; i < numOfThreads; ++i)
    {
        int start = i * chunkSize;
        int end = std::min(start + chunkSize, (int)words.size());
        threads.emplace_back(MapChunk, std::ref(words), start, end, std::ref(totalSum), std::ref(mutex));
    }

    for (auto& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
    return totalSum;
}

void MapReducing()
{
    std::vector<std::string> words =
    {
        "cat", "dog", "party", "music", "laughing", "computer", "science", "bachelor", "intelligent", "spelling", "bad", "sheep",
        "table", "candy", "donut", "pizza", "sushi", "coffee", "laptop", "cellphone", "cellophane", "anime", "movies", "volleyball"
    };

    StartAlgorithm();
    int numThreads = 1;
    int totalSum = WordLenghtSum(words, numThreads);
    std::cout << "Total Characters: " << totalSum << " in " << words.size() << " words.\n";
    PrintAlgorithmDuration();

    StartAlgorithm();
    numThreads = 3;
    totalSum = WordLenghtSum(words, numThreads);
    std::cout << "Total Characters: " << totalSum << " in " << words.size() << " words.\n";
    PrintAlgorithmDuration();
}

class TaskQueue
{
public:
    void Push(std::function<void(int)> task)
    {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mTasks.push(task);
        }
        mCV.notify_one();
    }

    bool Pop(std::function<void(int)>& task)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mCV.wait(lock, [this] { return mStop || !mTasks.empty(); });

        if (mStop && mTasks.empty())
        {
            return false;
        }

        task = std::move(mTasks.front());
        mTasks.pop();
        return true;
    }

    void Stop()
    {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mStop = true;
        }
        mCV.notify_all();
    }

private:
    std::queue<std::function<void(int)>> mTasks;
    std::mutex mMutex;
    std::condition_variable mCV;
    bool mStop = false;
};

void WorkerDoWork(TaskQueue& tasks, int id)
{
    std::function<void(int)> task;
    while (tasks.Pop(task))
    {
        task(id);
    }
}

void MasterWorker()
{
    StartAlgorithm();
    TaskQueue tasks;

    // Create Workers
    int numWorkers = 3;
    std::vector<std::thread> workers;
    for (int i = 0; i < numWorkers; ++i)
    {
        workers.emplace_back(WorkerDoWork, std::ref(tasks), i);
    }

    // Master Creates Tasks
    for (int i = 0; i < 40; ++i)
    {
        tasks.Push([](int id) {
            int workTime = (rand() % 200) + 50; // Simulate work time between 50ms & 250ms
            std::this_thread::sleep_for(std::chrono::milliseconds(workTime));
            std::lock_guard<std::mutex> lock(gPrintMutex);
            std::cout << "Work is done by worker ( " << id << " )\n";
            });
    }
    tasks.Stop();

    for (auto& w : workers)
    {
        if (w.joinable())
        {
            w.join();
        }
    }

    std::cout << "All tasks are complete!\n";
    PrintAlgorithmDuration();
}

void PrintRange(const std::vector<uint32_t>& numbers, int start, int end)
{
    for (int i = start; i < end; ++i)
    {
        std::cout << numbers[i] << " ";
    }
    std::cout << "\n";
}

//void StdExecution()
//{
//    // Create a vector of 10000 random numbers from 1 - 1000​
//    // Using parallel algorithms, sort them from highest to lowest​
//    // Add a random number between 1 - 40 to each​
//    // Sort them again from lowest to highest
//
//    int maxNumbers = 100000;
//    int minVal = 1;
//    int maxVal = 1000;
//    int seed = 47;
//
//    srand(seed);
//    std::vector<uint32_t> numbers;
//    for (int i = 0; i < maxNumbers; ++i)
//    {
//        int value = (rand() % maxVal) + minVal;
//        numbers.push_back(value);
//    }
//
//    StartAlgorithm();
//    std::cout << "Done In Sequence:\n";
//    PrintRange(numbers, 0, 20);
//    std::sort(std::execution::par, numbers.begin(), numbers.end(), std::greater<uint32_t>());
//    PrintRange(numbers, 0, 20);
//    std::transform(std::execution::seq, numbers.begin(), numbers.end(), numbers.begin(), [](int n) { return n + (rand() % 40) + 1; });
//    PrintRange(numbers, 0, 20);
//    std::sort(std::execution::seq, numbers.begin(), numbers.end());
//    PrintRange(numbers, 0, 20);
//    PrintAlgorithmDuration();
//    std::cout << "\n\n";
//
//    srand(seed);
//    numbers.clear();
//    for (int i = 0; i < maxNumbers; ++i)
//    {
//        int value = (rand() % maxVal) + minVal;
//        numbers.push_back(value);
//    }
//
//    StartAlgorithm();
//    std::cout << "Done In Parallel:\n";
//    PrintRange(numbers, 0, 20);
//    std::sort(std::execution::par, numbers.begin(), numbers.end(), std::greater<uint32_t>());
//    PrintRange(numbers, 0, 20);
//    std::transform(std::execution::par, numbers.begin(), numbers.end(), numbers.begin(), [](int n) { return n + (rand() % 40) + 1; });
//    PrintRange(numbers, 0, 20);
//    std::sort(std::execution::par, numbers.begin(), numbers.end());
//    PrintRange(numbers, 0, 20);
//    PrintAlgorithmDuration();
//}


int main()
{
    std::cout << "Parallel Algorithims!\n";

    // ForkJoin();

    // DivideAndConquer();

    // ProducerConsumer();

    // Pipeline();

    // WorkerSteal();

    // MapReducing();

    // MasterWorker();

    // StdExecution();
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
