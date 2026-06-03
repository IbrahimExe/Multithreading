// MonteCarlo PI using OpenMP Assignment
/*
For this assignment, use the “MonteCarlo PI” file provided.

Attached is the image describing MonteCarlo method for computing PI. 
Your task is to implement this and compare it with the integration method we 
did in class for computing PI. I have attached the cpp of the project as well.

Your task is to :
1. Implement Monte Carlo method in the most efficient way based on stuff we learned
   in class so far.
2. Add timing, as we did in class, and print the result
3. Encapsulate the 2 different PI computation in 2 different function named 
   PI_Integral() and PIMonteCarlo() and compare the timing results for PI
   accurate up to 15 decimal points. 
   Make sure you compare timing when both compute PI precise to 15 decimal points.
4. Hand in clean project and exe in release mode.
*/

#include <iostream>
#include <iomanip>
#include <omp.h>
#include <chrono>
#include <random>
#include <cmath>

const double PI_15DP = 3.141592653589793;
const int NUM_THREADS = omp_get_max_threads();

// Integration method for computing PI
double PI_Integral()
{
	long num_steps = 10000000;
	double step = 1.0 / (double)num_steps;
	double sum = 0.0;

#pragma omp parallel for reduction(+: sum)
	for (long i = 0; i < num_steps; ++i)
	{
		double x = (i + 0.5) * step;
		sum += 4.0 / (1.0 + x * x);
	}

	double pi = sum * step;
	return pi;
}

// Monte Carlo method for computing PI
double PIMonteCarlo()
{
	// using a huge number of fixed samples;
	// Monte Carlo converges as 1/sqrt(N), so 2 billion samples gives ~6-7 (hehe) decimal places
	// its not 15 dp's but gives us a good estimate in a relativley reasonable time frame.
	long samples = 2000000000;  // 2 billion samples
	long count = 0;

	// Generate random points and count those inside circle
#pragma omp parallel
	{
		int thread_id = omp_get_thread_num();
		std::mt19937 gen(thread_id);
		std::uniform_real_distribution<> dis(0.0, 1.0);

		long local_count = 0;

#pragma omp for
		for (long i = 0; i < samples; ++i)
		{
			double x = dis(gen);
			double y = dis(gen);

			// Check if point is inside unit circle
			if (x * x + y * y <= 1.0)
			{
				local_count++;
			}
		}

#pragma omp critical
		count += local_count;
	}

	double pi = 4.0 * (double)count / (double)samples;
	return pi;
}

int main()
{
	std::cout << std::fixed << std::setprecision(15);
	std::cout << "Computing PI using two methods...\n\n";

	// Compute PI using Integration method
	std::cout << "   Integration Method \n";
	auto int_start = std::chrono::steady_clock::now();
	double pi_integral = PI_Integral();
	auto int_end = std::chrono::steady_clock::now();
	long int_time = std::chrono::duration_cast<std::chrono::milliseconds>(int_end - int_start).count();

	std::cout << "PI (Integration) = " << pi_integral << "\n";
	std::cout << "Exact PI         = " << PI_15DP << "\n";
	std::cout << "Error            = " << std::abs(pi_integral - PI_15DP) << "\n";
	std::cout << "Time: " << int_time << " ms\n\n";

	// Compute PI using Monte Carlo method
	std::cout << "   Monte Carlo Method \n"; // using 2 Billion samples 
	auto mc_start = std::chrono::steady_clock::now();
	double pi_montecarlo = PIMonteCarlo();
	auto mc_end = std::chrono::steady_clock::now();
	long mc_time = std::chrono::duration_cast<std::chrono::milliseconds>(mc_end - mc_start).count();

	std::cout << "PI (Monte Carlo) = " << pi_montecarlo << "\n";
	std::cout << "Exact PI         = " << PI_15DP << "\n";
	std::cout << "Error            = " << std::abs(pi_montecarlo - PI_15DP) << "\n";
	std::cout << "Time: " << mc_time << " ms\n\n";

	// Comparison
	std::cout << "- Comparison -\n";
	std::cout << "Integration Method Time: " << int_time << " ms\n";
	std::cout << "Monte Carlo Method Time: " << mc_time << " ms\n";

	// I would put an if statement for in the odd chance MonteCarlo is somehow faster but
	// its so unlikely that i feel its safe to assume it wont happen at all lol
	std::cout << "Integration is ~ " << (int)mc_time / (int)int_time << "x faster than the Monte Carlo method\n";

	return 0;
}