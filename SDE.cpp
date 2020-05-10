#include "SDE.hpp"

// Parameters t and S not used alot, this could be extended to include 
// local/stochastic interest rate or volatility models
double SDE::drift(double t, double S)
{ 
	// Drift term	
	if (this->SDE_type == 0) // Euler
		return (data->r - data->D) * S; // r - D
	else                     // Exact
		return this->v;
}

double SDE::diffusion(double t, double S) 
{ 
	// Diffusion term
	return data->sigma * S;
}

std::tuple< std::vector<double>, std::vector<double>> SDE::generatePaths(double S, const std::vector<double> &dW)
{
	// Define and initialise variables 
	double dt = data->T / static_cast<double>(this->NT);
	double sqrdt = std::sqrt(dt);
	double t = 0.0;

	// Calculate a path at each iteration
	std::vector<double> path_plus;
	std::vector<double> path_minus;

	// Tuples to store plus and minus paths for AVR // TODO: COMMENT
	std::tuple <double, double> VOld;
	std::tuple <double, double> VNew;

	// Initialise tuples
	VOld = std::make_tuple(S, S);
	VNew = std::make_tuple(0.0, 0.0);

	// Loop through the number of time steps
	for (long index = 0; index < NT; ++index)
	{
		t += dt;
		// Euler
		if (SDE_type == 0)
		{
			std::get<0>(VNew) = std::get<0>(VOld) + drift(t, std::get<0>(VOld)) * dt 
				+ diffusion(t, std::get<0>(VOld)) * dW[index];
			std::get<1>(VNew) = std::get<1>(VOld) + drift(t, std::get<1>(VOld)) * dt
				- diffusion(t, std::get<1>(VOld)) * dW[index];
		}
		// Exact
		else if (SDE_type == 1)
		{
			std::get<0>(VNew) = std::get<0>(VOld) * std::exp(drift(t, 1.0) * dt
				+ diffusion(t, 1.0) * dW[index]);
			std::get<1>(VNew) = std::get<1>(VOld) * std::exp(drift(t, 1.0) * dt
				- diffusion(t, 1.0) * dW[index]);
		}

		// Add to vector
		path_plus.push_back(std::get<0>(VOld));
		path_minus.push_back(std::get<1>(VOld));

		// Update values
		std::get<0>(VOld) = std::get<0>(VNew);
		std::get<1>(VOld) = std::get<1>(VNew);
	}

	// Return both the path and the negated path as a tuple of vectors
	path_plus.push_back(std::get<0>(VOld));
	path_minus.push_back(std::get<1>(VOld));

	// Make tuple and return 
	std::tuple<std::vector<double>, std::vector<double>> returnTuple{ path_plus, path_minus };
	return returnTuple;
}