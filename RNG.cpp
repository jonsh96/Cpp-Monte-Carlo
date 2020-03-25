#include "RNG.hpp"
#include <random>

std::vector<std::vector<double>> RNG::generateWienerProcesses(double dt)
{
	// Generates Wiener processes and stores as a matrix
	// Normal (0,1) rng
	std::mt19937 rd;
	std::default_random_engine generator(rd()); // rd() provides a random seed
	std::normal_distribution<double> distribution(0, 1);

	// Initialise matrix of Wiener paths
	std::vector<std::vector<double>> temp_paths;

	double sqrdt = std::sqrt(dt);

	for (long i = 0; i <= this->M; ++i)
	{
		// Initialise temporary path
		std::vector<double> temp_path;
		for (long j = 0; j <= NT; ++j)
		{
			// Add a random normally distributed Wiener process to the Wiener path
			temp_path.push_back(sqrdt * distribution(generator));
		}
		temp_paths.push_back(temp_path);
	}
	// Set the member matrix dW as the paths generated
	return temp_paths;

}