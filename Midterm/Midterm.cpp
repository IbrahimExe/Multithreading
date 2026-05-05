// Midterm.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <chrono>
#include <queue>
#include <atomic> 

const std::size_t MaxQueueSize = 1000;
using WordCountMapType = std::unordered_map<std::string, std::size_t>;  // use this to keep track of count of each word.

struct WordCount // use this to track words to sort, this is an example of storing the count, but there are other options
{
	std::string word;
	std::size_t count;
};

struct WordQueue
{
	std::queue<std::string> words;
	std::mutex mutex;
	std::condition_variable notFull;
	std::condition_variable notEmpty;

	void push(const std::string& word)
	{
		std::unique_lock < std::mutex> lock(mutex);
		notFull.wait(lock, [this]() {return words.size() < MaxQueueSize; });
		words.push(word);

		lock.unlock();
		notEmpty.notify_one();
	}

	std::string pop()
	{
		std::unique_lock<std::mutex> lock(mutex);
		if (words.empty()) return "";
		std::string word = words.front();
		words.pop();
		lock.unlock();
		notFull.notify_one();
		return word;
	}

	// a more efficient pop
	bool waitAndPop(std::string& word, std::atomic<int>& finishedProducers, int totalProducers)
	{
		std::unique_lock<std::mutex> lock(mutex);

		notEmpty.wait(lock, [this, &finishedProducers, totalProducers]() {
			return !words.empty() || finishedProducers == totalProducers;
			});

		if (words.empty()) return false;

		word = words.front();
		words.pop();

		lock.unlock();
		notFull.notify_one();
		return true;
	}

	bool IsEmpty()
	{
		std::unique_lock<std::mutex> lock(mutex);
		return words.empty();
	}

	std::size_t Size()
	{
		std::unique_lock<std::mutex> lock(mutex);
		return words.size();
	}
};

std::vector<WordCount> GetTopWords(WordCountMapType& wordMap, int topN)
{
	std::vector<WordCount> vec;
	// map out the words and the counts
	for (const auto& pair : wordMap)
	{
		vec.push_back({ pair.first,pair.second });
	}

	// sort to only get the topN counts
	std::sort(vec.begin(), vec.end(), [](const WordCount& a, const WordCount& b)
		{
			return a.count > b.count;
		});

	if (vec.size() > topN)
	{
		vec.erase(vec.begin() + topN, vec.end());
	}

	return vec;
}

// This writes the word counts to the outputfile
void WriteOutput(const char* const filename, WordCountMapType& wordMap)
{
	std::ofstream out(filename);
	std::vector<WordCount> topWords = GetTopWords(wordMap, 20); // Top 20 most occuring words

	for (auto& data : topWords)
	{
		out << data.word << " : " << data.count << std::endl;
	}
	out.close();
}

WordCountMapType WordsInFile(const char* const fileName) // for each word
{ // in file, return
	std::ifstream file(fileName); // # of
	WordCountMapType wordCounts; // occurrences

	for (std::string word; file >> word;)// this is not efficient. You can read the whole file into one string and the process that string , instead of reading one word at a time from file.
	{
		// Only count words with size > 1
		if (word.size() > 1)
		{
			++wordCounts[word];
		}
	}
	file.close();
	return wordCounts;
}

// Read book file and push the words read into the queuw
void ProducerThread(const char* fileName, WordQueue& queue, WordCountMapType& bookWordMap, std::atomic<int>& finishedProducers)
{
	std::ifstream file(fileName);
	std::string word;

	while (file >> word)
	{
		if (word.size() > 1)
		{
			queue.push(word);
			++bookWordMap[word];
		}
	}
	file.close();

	finishedProducers++; 
	queue.notEmpty.notify_all();
}

// Mutex for protecting the main word map and console output
std::mutex masterMapMutex;
std::mutex consoleMutex;

int main(int argc, char* argv[])
{
	auto overallStart = std::chrono::steady_clock::now();

	std::cout << "Midterm Word Count:\n";
	std::cout << "Started...\n";

	if (argc < 2)
	{
		std::cout << "Progress: wordCounting.exe book1.txt book2.txt ...\n";
		return 1;
	}

	// master word map for all books combined
	WordCountMapType masterWordMap;
	masterWordMap.reserve(50000);

	// create a vector to hold all threads
	std::vector<std::thread> producerThreads;

	// create queues and word maps for each book
	std::vector<WordQueue> queues(argc - 1);
	std::vector<WordCountMapType> bookMaps(argc - 1);

	for (auto& m : bookMaps) m.reserve(20000); 

	std::atomic<int> finishedProducers(0);

	std::cout << "\nProgress: Starting analysis of " << (argc - 1) << " book(s)...\n";
	auto analysisStart = std::chrono::steady_clock::now();

	// create producer threads for each book
	for (int i = 1; i < argc; ++i)
	{
		std::cout << "Progress: Analysing " << argv[i] << "...\n";
		producerThreads.push_back(std::thread(ProducerThread, argv[i],
			std::ref(queues[i - 1]),
			std::ref(bookMaps[i - 1]),
			std::ref(finishedProducers)));
	}

	std::cout << "Progress: Collecting words from queues...\n";

	// Continuously collect from all queues while producers are running
	int totalCollected = 0;

	for (int i = 0; i < queues.size(); ++i)
	{
		WordCountMapType localBatch;
		localBatch.reserve(1000);

		while (true)
		{
			std::string word;

			if (!queues[i].waitAndPop(word, finishedProducers, argc - 1))
				break;

			localBatch[word]++;
			totalCollected++;

			if (localBatch.size() >= 1000)
			{
				std::lock_guard<std::mutex> lock(masterMapMutex);
				for (auto& p : localBatch)
					masterWordMap[p.first] += p.second;
				localBatch.clear();
			}
		}

		// flush remaining
		std::lock_guard<std::mutex> lock(masterMapMutex);
		for (auto& p : localBatch)
			masterWordMap[p.first] += p.second;
	}

	// wait for all producer threads to finish
	std::cout << "Progress: Waiting for producer threads to complete...\n";
	for (auto& thread : producerThreads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}

	auto analysisEnd = std::chrono::steady_clock::now();
	double analysisTime = std::chrono::duration<double>(analysisEnd - analysisStart).count();

	// write output for each book
	std::cout << "\nProgress: Writing outputs for each book...\n";
	for (int i = 1; i < argc; ++i)
	{
		std::string outputFileName = std::string(argv[i]) + "_output.txt";
		WriteOutput(outputFileName.c_str(), bookMaps[i - 1]);
		std::cout << "Progress: Wrote " << outputFileName << "\n";
	}

	// combined output for all books
	std::cout << "\nProgress: Writing combined output...\n";
	WriteOutput("combined_output.txt", masterWordMap);
	std::cout << "Progress: Wrote combined_output.txt\n";

	// top 20 words
	std::cout << "\nTop 20 most words across all books:\n";
	std::vector<WordCount> topWords = GetTopWords(masterWordMap, 20);
	for (const auto& wordCount : topWords)
	{
		std::cout << wordCount.word << " : " << wordCount.count << "\n";
	}

	auto overallEnd = std::chrono::steady_clock::now();
	double totalTime = std::chrono::duration<double>(overallEnd - overallStart).count();

	std::cout << "\nAnalysis time: " << analysisTime << " seconds\n";
	std::cout << "Total time: " << totalTime << " seconds\n";
	std::cout << "\nComplete\n";

	return 0;
}