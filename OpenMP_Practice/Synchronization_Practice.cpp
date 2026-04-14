#include <iostream>
#include <omp.h> // Open MP Library
#include <chrono> // For measuring time
#include <algorithm>
#include <random>
#include <iomanip> // for Setprecision

#define NUM_THREADS 4
static long numSteps = 10000000;
//double step = 0.0;

//void Example3()
//{
//    double pi = 0.0;
//    step = 1.0 / (double)numSteps;
//    omp_set_num_threads(NUM_THREADS); // Sets the number of parallel threads
//
//    // Current CPU time (in utc)
//    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
//
//#pragma omp parallel
//    {
//        double x = 0.0;
//        double sum = 0.0;
//        int i = 0;
//        int id = omp_get_thread_num(); // Get the thread ID
//        int nThreads = omp_get_num_threads(); // Get the total number of threads
//
//        for (i = id; i < numSteps; i += nThreads)
//        {
//            x = (i + 0.5) * step;
//            sum = sum + (4.0 / (1.0 + x * x));
//        }
//        for (i = id; i < numSteps; i += nThreads)
//        {
//            x = (i + 0.5) * step;
//            sum += (4.0 / (1.0 + x * x));
//        }
//
//#pragma omp atomic // Prevent other threads from updating 'pi' value
//        // Only lets one thread do the math at a time, 
//        // Prevents 
//
//#pragma omp critical // Letting thread know a shared value is being updated
//        {
//            pi += step * sum;
//        }
//    }
//
//    //printf("Pi is approximately: %.16f\n", pi);
//    std::cout << "Pi: " << pi << "\n";
//
//    std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
//    int duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
//    std::cout << "Execution Time: " << duration << " ms\n";
//
//    //return 0;
//}

double RandRange(double min, double max)
{
    double r = static_cast<double>(rand()) / RAND_MAX;
    return min + (max - min) * r;
}

void MonteCarlo()
{
    // Monte Carlo Pi Calculation
    long numTrials = 10000;
    long i = 0;
    long nCirc = 0; // Number of points that fall within the circle
    double pi = 0.0;
    double x = 0.0;
    double y = 0.0;
    double r = 1.0; // Radius of the circle. Side of square is 2*r
    std::srand(time(0));

#pragma omp parallel for private(x, y) reduction(+:nCirc) // Make a copy of it for each thread and merge it all togehter
    for (i = 0; i < numTrials; ++i)
    {
        x = RandRange(-r, r);
        y = RandRange(-r, r);
        // DistSq for 2D point
        if ((x * x + y * y) <= (r * r))
        {
            ++nCirc;
        }
    }
    pi = 4.0 * (static_cast<double>(nCirc) / static_cast<double>(numTrials));
    std::cout << std::setprecision(20) << "Pi: " << pi << "NumHits: " << nCirc << "\n";
}

void Example4_Sections()
{
    omp_set_num_threads(4);

#pragma omp parallel
    {
#pragma omp sections // adding 'nowait' here will allow the threads to continue without waiting for all sections to finish
        {
#pragma omp section
            {
                int id = omp_get_thread_num();
                for (int i = 0; i < 10; ++i)
                {
                    std::cout << "Section 1 for ID: " << id << "Index: " << i << "\n";
                }
            }
#pragma omp section
            {
                int id = omp_get_thread_num();
                for (int i = 0; i < 10; ++i)
                {
                    std::cout << "Section 2 for ID: " << id << "Index: " << i << "\n";
                }
            }
#pragma omp section
            {
                int id = omp_get_thread_num();
                for (int i = 0; i < 10; ++i)
                {
                    std::cout << "Section 3 for ID: " << id << "Index: " << i << "\n";
                }
            }
#pragma omp section
            {
                int id = omp_get_thread_num();
                for (int i = 0; i < 10; ++i)
                {
                    std::cout << "Section 4 for ID: " << id << "Index: " << i << "\n";
                }
            }
#pragma omp section
            {
                int id = omp_get_thread_num();
                for (int i = 0; i < 10; ++i)
                {
                    std::cout << "Section 5 for ID: " << id << "Index: " << i << "\n";
                }
            }
        }
    }

    std::cout << "All threads are done!\n";
}

struct Matrix
{
    float m[16];
    Matrix()
    {
        for (int i = 0; i < 16; ++i)
        {
            m[i] = 0;
            if (i == 0 || i == 5 || i == 10 || i == 15)
            {
                m[i] = 1;
            }
        }
    }
};
void MatrixMul(Matrix& a, Matrix& b, Matrix& result)
{
    // 4x4 Matrix
    int m = 4; // Number of rows in A and C
    int n = 4; // Number of columns in B and C
#pragma omp parallel for
    for (int i = 0; i < n; ++i)
    {
#pragma omp critical
        std::cout << "Thread: " << omp_get_thread_num() << "Index: " << i << "\n";
        for (int j = 0; j < m; ++j)
        {
            float cell = 0.0f;
            for (int k = 0; k < m; ++k)
            {
                int aIndex = k + (i * m); // Row-major order
                int bIndex = j + (k * n); // Row-major order
                cell += a.m[aIndex] * b.m[bIndex];
            }
            int resIndex = j + (i * m);
            result.m[resIndex] = cell;
        }
    }
}

int main()
{
    Matrix a;
    Matrix b;
    Matrix result;
    MatrixMul(a, b, result);
    for (int i = 0; i < 16; ++i)
    {
        std::cout << result.m[i] << " ";
        if ((i + 1) % 4 == 0)
        {
            std::cout << "\n";
        }
    }
    return 0;
}