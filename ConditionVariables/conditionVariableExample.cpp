// Assignment File
#include <mutex>
#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <thread>

std::atomic_int totalProduced(0);

struct CircularBuffer {
	int* buf;
	int capacity;

	int frontIdx, rearIdx, count;
	std::mutex m;
	std::condition_variable notEmpty, notFull;
	CircularBuffer(int cap) : capacity(cap), frontIdx(0), rearIdx(0), count(0), buf(new int[cap]) {}

	~CircularBuffer() { delete[] buf; }

	void push(int num)
	{
		std::unique_lock<std::mutex> lk(m);
		notFull.wait(lk, [this]() {return count != capacity; });
		buf[rearIdx] = num;
		rearIdx = (rearIdx + 1) % capacity;
		++count;

		lk.unlock();
		notEmpty.notify_all();
	}

	int pop()
	{
		std::unique_lock<std::mutex> lk(m);
		notEmpty.wait(lk, [this]() {return count > 0; });
		int data{ buf[frontIdx] };
		frontIdx = (frontIdx + 1) % capacity;
		--count;

		lk.unlock();
		notFull.notify_one();
		return data;
	}

};

std::mutex printLock;

void consumer1(CircularBuffer& buf)
{
	std::vector<int> numbers;
	int consumed = 0;

	while (consumed < 2500)
	{
		int val = buf.pop();
		numbers.push_back(val);
		consumed++;
	}

	int closest1 = -1, closest2 = -1;
	int minDiff = INT_MAX;

	for (int i = 0; i < numbers.size(); ++i)
	{
		for (int j = i + 1; j < numbers.size(); ++j)
		{
			int diff = abs(numbers[i] - numbers[j]);
			if (diff < minDiff)
			{
				minDiff = diff;
				closest1 = numbers[i];
				closest2 = numbers[j];
			}
		}
	}

	{
		std::lock_guard<std::mutex> lk(printLock);
		std::cout << "Consumer 1: closest numbers are " << closest1 << " and " << closest2
			<< " with difference " << minDiff << ". Total numbers: " << numbers.size() << std::endl;
	}
}

void consumer2(CircularBuffer& buf)
{
	std::vector<int> numbers;
	int consumed = 0;

	while (consumed < 2500)
	{
		int val = buf.pop();
		numbers.push_back(val);
		consumed++;
	}

	int closest1 = -1, closest2 = -1;
	int minDiff = INT_MAX;

	for (int i = 0; i < numbers.size(); ++i)
	{
		for (int j = i + 1; j < numbers.size(); ++j)
		{
			int diff = abs(numbers[i] - numbers[j]);
			if (diff < minDiff)
			{
				minDiff = diff;
				closest1 = numbers[i];
				closest2 = numbers[j];
			}
		}
	}

	{
		std::lock_guard<std::mutex> lk(printLock);
		std::cout << "Consumer 2: closest numbers are " << closest1 << " and " << closest2
			<< " with difference " << minDiff << ". Total numbers: " << numbers.size() << std::endl;
	}
}

void consumer3(CircularBuffer& buf)
{
	std::vector<int> numbers;
	int consumed = 0;

	while (consumed < 2500)
	{
		int val = buf.pop();
		numbers.push_back(val);
		consumed++;
	}

	int closest1 = -1, closest2 = -1;
	int minDiff = INT_MAX;

	for (int i = 0; i < numbers.size(); ++i)
	{
		for (int j = i + 1; j < numbers.size(); ++j)
		{
			int diff = abs(numbers[i] - numbers[j]);
			if (diff < minDiff)
			{
				minDiff = diff;
				closest1 = numbers[i];
				closest2 = numbers[j];
			}
		}
	}

	{
		std::lock_guard<std::mutex> lk(printLock);
		std::cout << "Consumer 3: closest numbers are " << closest1 << " and " << closest2
			<< " with difference " << minDiff << ". Total numbers: " << numbers.size() << std::endl;
	}
}

void consumer4(CircularBuffer& buf)
{
	std::vector<int> numbers;
	int consumed = 0;

	while (consumed < 2500)
	{
		int val = buf.pop();
		numbers.push_back(val);
		consumed++;
	}

	int closest1 = -1, closest2 = -1;
	int minDiff = INT_MAX;

	for (int i = 0; i < numbers.size(); ++i)
	{
		for (int j = i + 1; j < numbers.size(); ++j)
		{
			int diff = abs(numbers[i] - numbers[j]);
			if (diff < minDiff)
			{
				minDiff = diff;
				closest1 = numbers[i];
				closest2 = numbers[j];
			}
		}
	}

	{
		std::lock_guard<std::mutex> lk(printLock);
		std::cout << "Consumer 4: closest numbers are " << closest1 << " and " << closest2
			<< " with difference " << minDiff << ". Total numbers: " << numbers.size() << std::endl;
	}
}

void producer(CircularBuffer& buf)
{
	std::mt19937 generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());

	for (int i = 0; i < 10000; ++i)
	{
		//buf.push(generator() % 1000);    // 0 - 999
		buf.push(generator() % 100000000); // 0-99,999,999 = more variation
		totalProduced++;
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Generate 10,000 random integers 
																	// With a 10ms delay
	}
}

// Cant generate 8000 random ints, and leave quadrant consumers at 2500 each
// As this would cause a deadlock/

int main()
{
	CircularBuffer dataBuf(400);

	std::thread prod(producer, std::ref(dataBuf));
	std::thread cons1(consumer1, std::ref(dataBuf));
	std::thread cons2(consumer2, std::ref(dataBuf));
	std::thread cons3(consumer3, std::ref(dataBuf));
	std::thread cons4(consumer4, std::ref(dataBuf));

	prod.join();
	cons1.join();
	cons2.join();
	cons3.join();
	cons4.join();

	std::cout << "Done! Total numbers produced: " << totalProduced << std::endl;
	return 0;
}