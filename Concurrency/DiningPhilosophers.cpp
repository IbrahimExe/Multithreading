/*
            Dining and Debating Philosophers Assignment [Week 3 Assignment]
This assignment is a classic example of use of concurrency.
5 philosophers sitting around a table with only 5 forks equally spaced around table.
The philosophers take turns eating using the forks and while not eating, debating.
Each philosopher can eat only when he / she holds both forks.
Each eating takes 1 second for a philosopher.
We want to have them all eat for total of 1 minute.
So a fair game is when all philosophers have equal chance in terms of time to eat and think.
This means on average each philosopher eats around 12 turns spread along 60 min equally.
You are going to use basic thread synchronization method mutex to serialize access to a fork.
Create a class named Philosopher and add a method to Philosopher named eat()
that simulated the act of eating at each turn.
You are going to print a message saying something like "Philosopher 2 is eating using forks 2 and 3".
Then it should pause for 4 seconds, before attempting to eat again.
You can use the command sleep() for this purpose.

IMPORTANT!You must use only C++ std::thread and std::mutex to implement this solution.
No other C++ multi - threading constructs can be used.
Also make sure you use lock guard or unique lock for mutex management.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <sstream>
#include <future> // for std::async
#include <random>

class Fork
{
public:
    Fork(int id) : forkId(id) {}
    
    std::mutex& getMutex()
    {
        return forkMutex;
    }
    
    int getId() const
    {
        return forkId;
    }
    
private:
    int forkId;
    std::mutex forkMutex;
};

std::atomic<bool> running(true);
std::mutex printLock;

class Philosopher
{
public:
    Philosopher(int id, Fork* leftFork, Fork* rightFork)
        : philosopherId(id), leftFork(leftFork), rightFork(rightFork), eatCount(0)
    {
    }
    
    void run()
    {
        while (running)
        {
            think();
            eat();
        }
    }
    
    void eat()
    {
        // Always pick up forks in order of ID to avoid deadlock
        Fork* firstFork = (leftFork->getId() < rightFork->getId()) ? leftFork : rightFork;
        Fork* secondFork = (leftFork->getId() < rightFork->getId()) ? rightFork : leftFork;
        
        std::lock_guard<std::mutex> lock1(firstFork->getMutex());
        std::lock_guard<std::mutex> lock2(secondFork->getMutex());
        
        {
            std::lock_guard<std::mutex> printGuard(printLock);
            std::cout << "Philosopher " << philosopherId << " is eating using forks " 
                      << firstFork->getId() << " and " << secondFork->getId() << "\n";
        }
        
        eatCount++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    void think()
    {
        std::this_thread::sleep_for(std::chrono::seconds(4));
    }
    
    int getEatCount() const
    {
        return eatCount;
    }
    
private:
    int philosopherId;
    Fork* leftFork;
    Fork* rightFork;
    int eatCount;
};

int main()
{
    std::cout << "Dining Philosophers Dilemma\n";

    // Create 5 forks
    Fork* forks[5];
    for (int i = 0; i < 5; ++i)
    {
        forks[i] = new Fork(i);
    }
    
    // Create 5 philosophers
    Philosopher* philosophers[5];
    for (int i = 0; i < 5; ++i)
    {
        philosophers[i] = new Philosopher(i, forks[i], forks[(i + 1) % 5]);
    }
    
    // Create threads for each philosopher
    std::thread threads[5];
    for (int i = 0; i < 5; ++i)
    {
        threads[i] = std::thread(&Philosopher::run, philosophers[i]);
    }
    
    // Let them run for 60 seconds
    std::this_thread::sleep_for(std::chrono::seconds(60));
    
    // Signal threads to stop
    running = false;
    
    // Wait for all threads to finish
    for (int i = 0; i < 5; ++i)
    {
        threads[i].join();
    }
    
    std::cout << "\nDone! Philosopher stats:\n";
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "Philosopher " << i << " ate " << philosophers[i]->getEatCount() << " times\n";
    }
    
    // Clean up
    for (int i = 0; i < 5; ++i)
    {
        delete philosophers[i];
        delete forks[i];
    }
    
    return 0;
}