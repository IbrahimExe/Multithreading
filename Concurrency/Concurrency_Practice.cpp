// Concurrency.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <future> // for std::async

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

    getchar(); // Basically wait for an input(Enter) before proceeding

    f.wait();
}

int main()
{
    // Default is std::launch::async, which means it will run the function in a separate thread immediately.
    // std::launch::async = launch when the async is created
    // std::launch::deferred = launch when the info is requested (when you call get() or wait())
    auto f = std::async(std::launch::deferred, WriteMessage, "Hello from another threaaaadd! (std::async)\n");
    
    WriteMessage("Hello from the main thread!\n");

    getchar(); // Basically wait for an input(Enter) before proceeding

    f.wait();
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
