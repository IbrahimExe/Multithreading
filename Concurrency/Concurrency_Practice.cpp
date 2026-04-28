// Week 3 - Concurrency
#include <iostream>
#include <future> // for std::async
#include <thread> // for std::thread
#include <sstream> // for std::stringstream
#include <chrono>
#include <random>
#include <vector>
#include <mutex>
#include "LockFreeStack.h"

void WriteMessage(const std::string& message)
{
    std::cout << message << "\n";
}

void Example1()
{
    auto f = std::async(WriteMessage, "Hello from another threaaaadd! (std::async)\n");

    for (int i = 0; i < 50; ++i)
    {
        WriteMessage("Hello from the MAIN thread!\n");
    } // Async is going to spin of a different task, while the main line will continue to do what it needs.

    f.wait(); // must be called to wait for the async task to finish before exiting the program,
              //otherwise it may terminate before the async task has a chance to complete its work.
 // f.get();  // Or this
}

void Example2()
{
    std::thread t(WriteMessage, "Hello from another thread! (std::thread)\n");
    for (int i = 0; i < 50; ++i)
    {
        WriteMessage("Hello from the MAIN thread!\n");
    }

    t.join(); // Wait for the thread to finish before exiting the program
}

void Example3()
{
    // Default is std::launch::async, which means it will run the function in a separate thread immediately.
    // std::launch::async = launch when the async is created
    // std::launch::deferred = launch when the info is requested (when you call get() or wait())
    auto f = std::async(std::launch::deferred, WriteMessage, "Hello from another threaaaadd! (std::async)\n");

    WriteMessage("Hello from the MAIN thread!\n");

    //getchar(); // Basically wait for an input(Enter) before proceeding
    (void)getchar(); // To prevent unused function warning,
    // or use when you don't care about the return value of a function

    f.wait();
}

int FindTheAnswer()
{
    return 42;
}

void Example4()
{
    auto f = std::async(FindTheAnswer);

    std::cout << "The meaning of life is: " << f.get() << "\n"; // get() will block until the result is ready, and then return the value.
}

std::string CopyString(const std::string& str)
{
    return str;
}

void Example5()
{
    // Passing parameters to async tasks
    std::string s = "Hello, World!";
    auto f = std::async(std::launch::async, CopyString, std::ref(s));
    // s = just the string
    
    // std::ref(s) = reference to the string, so the async task will capture the reference to 's' 
    // and will see any changes made to 's' after the async task is launched.

    // auto f = std::async(std::launch::deferred, [&s]() { return CopyString(s); }); 
    // = capture the reference to 's' in a lambda function, 
    // and the async task will execute the lambda function when get() or wait() is called, 
    // so it will see the value of 's' at that time.

    s = "Goodbye, World!"; // Modifying the original string after launching the async task
    std::cout << f.get() << "\n"; 
    // This will print "Hello, World!" because the async task 
    // captures the value of 's' at the time it was launched, 
    // not the reference to 's'.
}

void FindThePromiseAnswer(std::promise<int>* p)
{
    p->set_value(42); // Set the value of the promise, which will make it available to the future.
}

void Example6()
{
    std::promise<int> p; // a promise is an object that can be used to store a value that will be available in the future.
    auto f = p.get_future();
    std::thread t(FindThePromiseAnswer, &p); // Pass the promise by reference to the thread function
    std::cout << "the answer is: " << f.get() << "\n";
    // This will block until the promise is fulfilled and the value is set.

    t.join(); // Wait for the thread to finish before exiting the program
}

void Example7()
{
    std::packaged_task<int()> task(FindTheAnswer); // Wrap the function in a packaged_task
    auto f = task.get_future(); // Get the future associated with the task
    std::thread t(std::move(task)); // Move the task into a new thread and execute it
    std::cout << "The meaning of life is: " << f.get() << "\n"; // Wait for the result and print it
    t.join(); // when using threads, its generally t, wait for it to join back to main
}

void WaitForNotify(int id, std::shared_future<int> sf) // Wait for other threads
{
    std::ostringstream os;
    os << "Thread " << id << " is waiting for the signal...\n";
    std::cout << os.str();
    os.str(""); // Clear the stringstream for reuse
    os << "Thread " << id << " woken, val = " << sf.get() << "\n";
    std::cout << os.str();
}

void Example8()
{
    // Basically, thread 2 will wait until Enter is pressed,
    // and then both thread 1 and thread 2 will wake up and print their messages.
    std::promise<int> p;
    auto sf = p.get_future().share(); // Convert the future to a shared_future,
    // which can be shared among multiple threads

    std::thread t1(WaitForNotify, 1, sf); // Thread 1 will wait for the signal
    std::thread t2(WaitForNotify, 2, sf); // Thread 2 will also wait for the signal

    std::cout << "Waiting\n";
    std::cin.get(); // Wait for user input before sending the signal
    p.set_value(42); // Set the value of the promise, which will wake up both threads
    t2.join(); // Wait for thread 2 to finish
    t1.join(); // Wait for thread 1 to finish
}

std::mutex myMutex;
void MyFunctionThatUsesMutex()
{
    /*
    myMutex.lock(); // Lock the mutex to protect the critical section
    std::cout << "In My Function That Uses Mutex\n";
    myMutex.unlock(); // Unlock the mutex after the critical section is done
    */

    std::lock_guard<std::mutex> myGuard(myMutex); // Lock the mutex when the lock_guard is created,
    // and automatically unlock it when the lock_guard goes out of scope
    std::cout << "In My Function That Uses Mutex with lock_guard\n";
}

void Example9()
{
    myMutex.lock(); // Lock the mutex before calling the function that uses it
    std::thread t(MyFunctionThatUsesMutex); // Start a new thread that will call the function that uses the mutex
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "In Main Thread\n";
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for a while to simulate work
    }

    myMutex.unlock(); // Unlock the mutex after the main thread is done with its work
    t.join();
}

void FunctionThatUsesUniqueLock(int i)
{
    std::unique_lock<std::mutex> guard(myMutex); // Lock the mutex when the unique_lock is created
    std::cout << "In Function That Uses Unique Lock (" << i << ")\n";
    
    guard.unlock(); // Manually unlock the mutex before the function ends
    std::this_thread::sleep_for(std::chrono::seconds(1));
    guard.lock(); // Lock the mutex again before the function ends
    std::cout << "In function (" << i << ") AGAIN\n";
}

void Example10()
{
    std::unique_lock<std::mutex> guard(myMutex); // Lock the mutex in the main thread
    std::thread t1(FunctionThatUsesUniqueLock, 1); // Start a new thread that will call the function that uses the unique_lock
    std::thread t2(FunctionThatUsesUniqueLock, 2); // Start another thread that will call the function that uses the unique_lock

    std::cout << "In Main Thread\n";

    std::this_thread::sleep_for(std::chrono::seconds(1));
    guard.unlock(); // Unlock the mutex to allow the other threads to proceed
    t1.join(); // Wait for thread 1 to finish
    t2.join(); // Wait for thread 2 to finish
}

class Account
{
public:
    Account(int balance) : balance(balance) {}
    friend void Transfer(Account& from, Account& to, int amount)
    {
        //std::lock_guard<std::mutex> lockFrom(from.mutex); // Lock the mutex of the 'from' account
        //std::lock_guard<std::mutex> lockTo(to.mutex); // Lock the mutex of the 'to' account
        //from.balance -= amount; // Transfer the amount from the 'from' account
        //to.balance += amount; // Transfer the amount to the 'to' account

        std::lock(from.mutex, to.mutex); // Lock both mutexes without risking deadlock
        std::lock_guard<std::mutex> lockFrom(from.mutex, std::adopt_lock); // Adopt the lock for the 'from' account
        std::lock_guard<std::mutex> lockTo(to.mutex, std::adopt_lock); // Adopt the lock for the 'to' account
        from.balance -= amount; // Transfer the amount from the 'from' account
        to.balance += amount; // Transfer the amount to the 'to' account
    }

    int GetBalance() const
    {
        return balance; // for printing purposes
    }

private:
    std::mutex mutex;
    int balance = 0;
};

void Example11()
{
    // Locking multiple mutexes
    Account ibrahim(100000000);
    Account santi(10);
    Transfer(ibrahim, santi, 10);

    std::cout << "Ibrahim's balance: " << ibrahim.GetBalance() << "\n";
    std::cout << "Santi's balance: " << santi.GetBalance() << "\n";
}

// Lock Free Stack Example
std::mutex gPrintLock; // used for serializing output stream
bool gDone = false;
LockFreeStack<unsigned> gErrorCodes; // stack of error codes issues by running threads

// Run by logger threads, which pop errors from the stack & process them (print them)
void LoggerFunction()
{
    // print a startup message
    std::unique_lock<std::mutex> lock(gPrintLock);
    std::cout << "[Logger]\t running...\n";
    lock.unlock();
    // While there are error codes in the stack, process them
    while (!gDone)
    {
        if (!gErrorCodes.Empty())
        {
            auto code = gErrorCodes.Pop();
            if (code != nullptr) // If there IS something
            {
                lock.lock(); // lock to prevent other threads from accessing this
                std::cout << "[Logger]\t processing error code: " << *code << "\n";
                lock.unlock(); // unlock to make this section available again
            }
        }
    }
}

// Run by worker threads, which runs processes that may trigger error codes
void WorkerFunction(int id, std::mt19937& randGen) // mt19937 is a random number generator
{
    // Print a startup message
    std::unique_lock<std::mutex> lock(gPrintLock);
    std::cout << "[Worker " << id << "]\t running...\n";
    lock.unlock();

    while (!gDone)
    {
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::seconds(1 + randGen() % 5)); // Sleep for a random time between 1 and 5 seconds
        // Simulate an error
        unsigned errorCode = id * 100 + (randGen() % 50); // Generate a random error code based on the worker ID
        gErrorCodes.Push(errorCode); // Push the error code onto the stack
    }
}

void Assignment_DiningPhilosophers()
{

}

//int main()
//{
//    std::mt19937 randomGenerator((unsigned)std::chrono::system_clock::now().time_since_epoch().count()); // Random number generator seeded with the current time
//    std::vector<std::thread> threads;
//
//    // Create all logger functions and add to vector
//    for (int i = 0; i < 5; ++i)
//    {
//        threads.push_back(std::thread(LoggerFunction));
//    }
//
//    // Create all worker functions and add to vector
//    for (int i = 0; i < 5; ++i)
//    {
//        // Add in parameters (id) as well as reference to the random generator
//        //  with the reference, as it is being called, it updates
//        //   otherwise each thread will have the exact same random outcomes
//        threads.push_back(std::thread(WorkerFunction, i + 1, std::ref(randomGenerator)));
//    }
//
//    // Simulate running the main thread / application for 30 seconds
//    std::this_thread::sleep_for(std::chrono::seconds(30));
//
//    gDone = true;
//    for (auto& t : threads)
//    {
//        t.join(); // Wait for ALL threads to finish before exiting the program
//    }
//    return 0;
//}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu