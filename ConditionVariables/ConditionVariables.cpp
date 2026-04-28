// Week 4 - Condition Variables
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>

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
            " at position (" << mX << ", " << mY << ", " << mZ << ")\n";
    }

    void GetName()
    {

    }

private:
    std::string mName = "";
    float mX = 0.0f, mY = 0.0f, mZ = 0.0f;
    bool mHasRenderData = false;
};

class EntityManager
{
public:
    void AddEntity(Entity& entity)
    {
        mEntities.push_back(entity);
    }

    void RemoveEntity(Entity& entity)
    {
        auto iter = std::find_if(mEntities.begin(), mEntities.end(), [&entity];
    }

    std::vector<Entity>& GetEntities() { return mEntities; }

private:
    std::vector<Entity> mEntities;
};

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
            std::vector<Entity>& entities = mEntityManager->GetEntities();
            for (Entity& e : entities)
            {
                e.Update(deltaTime);
            }
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
            {
                // Update(deltaTime);
                // frameDuration -= deltaTime;
            }
            // sleep_for(sleepTime);
        }
    }

    void Terminate()
    {
        mIsRunning = false;
    }

private:
    EntityManager mEntityManager = nullptr;
    std::mutex* mGameMutex = nullptr;
    bool mIsRunning = false;
};

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
        std::chrono::milliseconds sleepTime(500);
        while (mIsRunning)
        {
            system("cls"); // Clear the console
            mGameMutex->lock();
            std::vector<Entity>& entities = mEntityManager->GetEntities();
            for (Entity& e : entities)
            {
                e.Render();
            }
            mGameMutex->unlock();
            std::this_thread::sleep_for(sleepTime);
        }
    }

    void Terminate()
    {
        mIsRunning = false;
    }

private:
    EntityManager mEntityManager = nullptr;
    std::mutex* mGameMutex = nullptr;
    bool mIsRunning = false;
};

float RandFloat()
{
    float val = (rand() % 201) - 100; // Random float between -100 and 100
    return val;
}

int main()
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

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu