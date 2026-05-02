// Week 4 - Condition Variables
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <random>
#include <future>

/*
The Game Engine​
    Create a class Entity​
    float x, y, z; ​
    bool hasRenderData; ​
    string name​
    Update(float deltaTime); // move in a direction, update position, print name, distance moved, and position​
    Render(); // prints what entity is being rendered and current position​
*/
class Entity
{
public:
    Entity(const std::string& name, float x, float y, float z)
        : mName(name)
        , mX(x)
        , mY(y)
        , mZ(z)
    {

    }

    void Update(float deltaTime)
    {
        float moveX = (rand() % 21) - 10; // Random movement between -10 and 10
        float moveZ = (rand() % 21) - 10; // Random movement between -10 and 10
        float distance = sqrtf(moveX * moveX + moveZ * moveZ);
        if (std::abs(distance) > 0.01f)
        {
            moveX /= distance; // Normalize the movement vector
            moveZ /= distance;
        }
        mX += moveX * deltaTime; // Move the entity based on deltaTime
        mZ += moveZ * deltaTime;
        std::cout << "Update: " << mName << 
            " moved a distance of (" << distance << ")\n";
    }

    void Render()
    {
        std::cout << "Render: " << mName <<
            " is at position (" << mX << ", " << mY << ", " << mZ << ")\n";
    }

    const std::string& GetName() const { return mName; }

private:
    std::string mName = "";
    float mX = 0.0f, mY = 0.0f, mZ = 0.0f;
    bool mHasRenderData = false;
};

/*
Create a class (Singleton if you want practice) EntityManager​
    std::vector<Entity> mEntities; ​
    AddEntity(Entity & entity); ​
    RemoveEntity(const std::string & name); ​
    Entity& GetEntities();
*/
class EntityManager
{
public:
    void AddEntity(Entity& entity)
    {
        mEntities.push_back(entity);
    }

    void RemoveEntity(Entity& entity)
    {
        auto iter = std::find_if(mEntities.begin(), mEntities.end(), [entity](Entity& e)
            {
                return e.GetName() == entity.GetName();
            });
        if (iter != mEntities.end())
        {
            mEntities.erase(iter);
        }
    }

    std::vector<Entity>& GetEntities() { return mEntities; }

private:
    std::vector<Entity> mEntities;
};

/*
Create two class​
    Simulation​
        EntityManager* mEntityManager; ​
        std::mutex* mMutex; ​
        Initialize(EntityManager & em, std::mutex & mutex); 
         pass in variable if not using singleton, store as a EntityManager* mEntityManager​
        Update(); 
         Update that calls all of the entities update with a 1/60 deltaTime 
         is a thread and will sleep_for() 17 milliseconds (roughly 60 fps) ​
*/
class Simulation
{
public:
    void Initialize(EntityManager& em, std::mutex& mutex)
    {
        mEntityManager = &em;
        mGameMutex = &mutex;
        mIsRunning = true;
    }

    void Update()
    {
        std::chrono::milliseconds sleepTime(1000); // Simulate ~60 FPS
        const float deltaTime = 1.0f / 60.0f; // Assuming 60 FPS, so deltaTime is 1/60 seconds
        while (mIsRunning)
        {
            mGameMutex->lock();
            system("cls"); // Clear the console
            std::cout << "Running Update\n";
            std::vector<Entity>& entities = mEntityManager->GetEntities();
            for (Entity& e : entities)
            {
                e.Update(deltaTime);
            }
            std::this_thread::sleep_for(sleepTime);
            mGameMutex->unlock();
            std::this_thread::sleep_for(sleepTime);
            // If you were to make a game frame based,
            // after each loop, you would do a time check,
            // then loop for the frames to catch up to the current time

            // auto startTime = std::chrono::steady_clock::now();
            //  Update(deltaTime);
            // auto updateDuration = std::chrono::steady_clock::now() - startTime;
            // sleep_for(sleepTime - updateDuration); (If updated < 60fps)
            // if (renderUpdate > 60fps) { }
            // while (frameDuration >= deltaTime)
            //{
                // Update(deltaTime);
                // frameDuration -= deltaTime;
            //}
            // sleep_for(sleepTime);
        }
    }

    void Terminate()
    {
        mIsRunning = false;
    }

private:
    EntityManager* mEntityManager = nullptr;
    std::mutex* mGameMutex = nullptr;
    bool mIsRunning = false;
};

/*
Render​
    EntityManager* mEntityManager; ​
    std::mutex* mMutex; ​
    Initialize(EntityManager & em, std::mutex & mutex); 
    pass in variable if not using singleton, store as a EntityManager* mEntityManager​
    Render(); 
    Renders all of the entities
    is a thread and will sleep_for() 33 ms (roughly 30 fps), 
    use system("cls") each update​
*/
class Render
{
public:
    void Initialize(EntityManager& em, std::mutex& mutex)
    {
        mEntityManager = &em;
        mGameMutex = &mutex;
        mIsRunning = true;
    }

    void RenderEntities()
    {
        std::chrono::milliseconds sleepTime(1000);
        while (mIsRunning)
        {
            mGameMutex->lock();
            system("cls"); // Clear the console
            std::cout << "Running Render\n";
            std::vector<Entity>& entities = mEntityManager->GetEntities();
            for (Entity& e : entities)
            {
                e.Render();
            }
            std::this_thread::sleep_for(sleepTime);
            mGameMutex->unlock();
            std::this_thread::sleep_for(sleepTime);
        }
    }

    void Terminate()
    {
        mIsRunning = false;
    }

private:
    EntityManager* mEntityManager = nullptr;
    std::mutex* mGameMutex = nullptr;
    bool mIsRunning = false;
};

/*
Test​
    Create EntityManager, Simulation, and Render classes​
    Add 20 entities to the entity manager(give names and random positions xz between –100.0f and 100.0f with a y 0.0f)​
    Create threads for Simulation and Render(thread simulationThread(&Simulation::Run, &simulation);)​
    Run until you press a button to exit(system("pause"))​
*/

float RandFloat()
{
    // Creates a random float between -100 and 100
    float val = (rand() % 201) - 100; 
    return val;
}

void GameLoopExample()
{
    std::cout << "Game Simulation!\n";

    std::mutex gameMutex;
    EntityManager entityManager;
    Simulation simulation;
    Render render;

    simulation.Initialize(entityManager, gameMutex);
    render.Initialize(entityManager, gameMutex);

    for (int i = 0; i < 4; ++i) // Add 4 players to the game
    {
        std::string name = "Player " + std::to_string(i);
        Entity newEntity(name, RandFloat(), 0.0f, RandFloat());
        entityManager.AddEntity(newEntity);
    }

    for (int i = 0; i < 16; ++i) // Add 16 enemies to the game
    {
        std::string name = "Enemy " + std::to_string(i);
        Entity newEntity(name, RandFloat(), 0.0f, RandFloat());
        entityManager.AddEntity(newEntity);
    }

    std::thread simThread(&Simulation::Update, &simulation);
    std::thread renderThread(&Render::RenderEntities, &render);

    system("pause"); // Wait for user input before terminating the simulation
    simulation.Terminate(); // flag the thread to end
    render.Terminate();

    renderThread.join();
    simThread.join();
}

// thread safe flag to indicate data input is finished
std::atomic_bool done = false;

struct CircularBuffer
{
    int* buffer;
    int capacity;

    int frontIndex;
    int rearIndex;
    int count;
    std::mutex mutex;
    std::condition_variable notEmpty;
    std::condition_variable notFull;

    CircularBuffer(int cap)
        : capacity(cap)
        , frontIndex(0)
        , rearIndex(0)
        , count(0)
        , buffer(new int[cap])
    {

    }

    void Push(int num)
    {
        std::unique_lock<std::mutex> lk(mutex);
        // waits until condition is met to trigger
        notFull.wait(lk, [this]() { return count != capacity; });
        buffer[rearIndex] = num;
        rearIndex = (rearIndex + 1) % capacity;
        ++count;

        lk.unlock();
        // trigger sending a message it is not empty
        notEmpty.notify_one();
    }
    int Pop()
    {
        std::unique_lock<std::mutex> lk(mutex);
        notEmpty.wait(lk, [this]() { return count > 0; });
        int data = buffer[frontIndex];
        frontIndex = (frontIndex + 1) % capacity;
        --count;

        lk.unlock();
        notFull.notify_one();
        return data;
    }
};

std::mutex bufferMutex;
int totalMin = INT_MAX;
int totalMax = INT_MIN;
void ConsumerFunction(CircularBuffer& buffer, int id)
{
    int localMin = INT_MAX;
    int localMax = INT_MIN;
    for (int i = 0; i < 950; ++i)
    {
        int value = buffer.Pop();
        localMin = std::min(localMin, value);
        localMax = std::max(localMax, value);
    }
    {
        std::lock_guard<std::mutex> lk(bufferMutex);
        std::cout << "Consumer " << id << ": local max [" << localMax << "] local min [" << localMin << "]\n";
        totalMin = std::min(localMin, totalMin);
        totalMax = std::max(localMax, totalMax);
    }
}

void ProducerFunction(CircularBuffer& buffer, int id)
{
    auto sleepTime = std::chrono::milliseconds(10);
    std::mt19937 randomGenerator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
    for (int i = 0; i < 1000; ++i)
    {
        buffer.Push(randomGenerator() % 100000);
        std::this_thread::sleep_for(sleepTime);
    }
}

void Exercise2()
{
    CircularBuffer dataBuffer(400);
    std::thread consumer1(ConsumerFunction, std::ref(dataBuffer), 1);
    std::thread consumer2(ConsumerFunction, std::ref(dataBuffer), 2);
    std::thread producer1(ProducerFunction, std::ref(dataBuffer), 1);
    std::thread producer2(ProducerFunction, std::ref(dataBuffer), 2);

    producer2.join();
    producer1.join();
    consumer2.join();
    consumer1.join();

    std::cout << "Total Max: [" << totalMax << "] Total Min: [" << totalMin << "]\n";
}

struct Result
{
    int minValue = INT_MAX;
    int maxValue = INT_MIN;
};

Result ConsumerFunctionAsync(CircularBuffer& buffer, int id)
{
    Result result;
    for (int i = 0; i < 950; ++i)
    {
        int value = buffer.Pop();
        result.minValue = std::min(result.minValue, value);
        result.maxValue = std::max(result.maxValue, value);
    }

    return result;
}


int main()
{
    CircularBuffer dataBuffer(400);
    //std::async<Result> consumer1(ConsumerFunction, std::ref(dataBuffer), 1);
    std::future<Result> consumer1Result = std::async(std::launch::async, ConsumerFunctionAsync, std::ref(dataBuffer), 1);
    std::future<Result> consumer2Result = std::async(std::launch::async, ConsumerFunctionAsync, std::ref(dataBuffer), 2);
    std::thread producer1(ProducerFunction, std::ref(dataBuffer), 1);
    std::thread producer2(ProducerFunction, std::ref(dataBuffer), 2);

    Result result1 = consumer1Result.get();
    Result result2 = consumer2Result.get();
    producer2.join();
    producer1.join();

    std::cout << "Result 1 Max: [" << result1.maxValue << "] Min: [" << result1.minValue << "]\n";
    std::cout << "Result 2 Max: [" << result2.maxValue << "] Min: [" << result2.minValue << "]\n";
    totalMax = std::max(totalMax, result1.maxValue);
    totalMax = std::max(totalMax, result2.maxValue);
    totalMin = std::min(totalMin, result1.minValue);
    totalMin = std::min(totalMin, result2.minValue);

    std::cout << "Total Max: [" << totalMax << "] Total Min: [" << totalMin << "]\n";
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

/* Class Notes

Deadlock real life example:
Salmon has the contract but needs the pen held by Ibrahim.Ibrahim has the pen but needs the contract held by Salmon to sign it.
Neither will release their item until they have both.

The example in the GameLoopExample function is an example of a frame based game loop, 
where the game updates and renders at a fixed rate (e.g., 60 FPS).

*/

/* Assignment
For this assignment, use the “Condition Variable Example” file provided.

The file is a sample project for min max integer stream we developed in class. 
I have made some minor changes. The point is to use a buffer to be used as a circular queue.
Numbers are pushed on one end and used (consumed) on the other end.
You are going to use similar system to solve the following problem:

There is a particle system that generates
a stream of 2d points (x, y) randomly in range -1000.f to 1000.f for each component x, y.

The points are generated, with a 30 ms time pause between any two consecutive point generation.
As points are generated we like to find the closest 2 points in the buffer at each quarter.

There are 4 quarters in 2d space with respect to x, y axis: (+, +), (-, +), (-, -), (+, -).
For this create 4 consumer threads.
Each thread receives the newly created point from the common circular buffer, 
saves it in its own local vector, 
and then goes about finding the closest 2 points among the points in the local vector.

At the end 
- the closest 2 points, 
- their distance, 
- and total number of points in this quarter are printed to the output, 
like:
>quarter 1: closest points are (2.5, 3.1) and (10.1, 11.2) and their distance is 5.4.Total number of point in this quarter is 256.

Make sure to use async to create and manage the 4 threads

This whole engine should stop when the total number of points created reaches 10,000.

As usual hand in clean project with exe in release mode.
*/


/* Assignment [DONT DO THIS ONE, Uses Vector3, We just need to use Vector2]
Attached is the project for min max integer stream we developed in class. 

I have made some minor changes. The main point is to use a buffer to be used as a circular queue. 

Numbers are pushed on one end and used(consumed) on the other end.

Study the code carefully and analyze it so you know how it works. 
Notice that in consumer() threads do not go all the way to full 1000 count in the for loop.
This is important.
Try to figure out why and what problem it causes if we do so.

You are going to use similar system to solve the following problem:
Below is a problem you are going to solve using the above project.
There is a particle system that; 
generates a stream of 3d points (x, y, z) randomly in range 0.f to 100.f for each component x, y, and z.
We save the points as they are generated, with a 15 ms time pause between any two consecutive point generation, in a buffer.
Use sleep_for() function to cause the pause.
As points are generated we like to find the closest 2 points in the buffer at the moment.
For this create 3 consumer threads.
Each thread saves out the entire content of the buffer to a local array and then finds the closest 2 points among the local points.
The thread saves the closest 2 points to an outputBuffer which is created in the main thread and passed to the consumer threads.
The main thread print out the newly added pair of points.

So the output would look like this:
closest pair: <22, 31> and <20, 30> and their distance is sqrt(5)
closest pair: <15, 11> and <16, 8> and their distance is sqrt(10)
..........................................
..........................................



Do this for a stream of 100 print outs. Notice that the distance should continue shrink.
It is important to use the BoundedBuf or CircularBuf provided in the attached code or we did develop in class.

As usual hand in clean project with exe in release mode.
*/