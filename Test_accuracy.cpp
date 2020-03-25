#include <iostream>
#include "MonteCarlo.hpp"
#include "OptionData.hpp"
#include "FairValue.hpp"
#include "DataProcessing.hpp"

#include <random>
#include <iostream>
#include <vector>

/*
// Returns an accurate price of an option, using M = 1,000,000
int main()
{
	// Define variable
	double Smin, Smax, dS, K, T, r, sigma, D, alpha, accuracy;
	char type;
	int style;
	long NT, M;

	// Initialise variables
	Smin = 10.0;
	Smax = 10.0;
	dS = 0.5;		// dS > 0, otherwise infinite for-loop in MonteCarlo::generatePrices
	K = 9.5;
	T = 1.0;
	r = 0.05;
	sigma = 0.25;
	D = 0.01;
	alpha = 0.05;
	accuracy = 0.01;
	type = 'C';
	style = 0;
	NT = 250;
	M = 1000000;

	// Store option data
	OptionData OD(Smin, K, T, r, sigma, D, type, style);

	// Create two instances of Monte Carlo, exact and Euler method
	MonteCarlo MC_euler(OD, Smin, Smax, dS, NT, M, alpha, accuracy, 0, style);
	MonteCarlo MC_exact(OD, Smin, Smax, dS, NT, M, alpha, accuracy, 1, style);

	std::cout << "Running the Euler method\n";
	MC_euler.run();
	std::cout << "Running the exact method\n";
	MC_exact.run();

	// Printing results
	std::cout << "\t\tEuler\t\tExact\n";
	std::cout << "Option price:\t" << MC_euler.getOptionPrice() << "\t\t" << MC_exact.getOptionPrice() << "\n";
	std::cout << "Time elapsed:\t" << MC_euler.getTimeElapsed() <<"\t\t" << MC_exact	.getTimeElapsed() << "\n";

	return 0;
}*/