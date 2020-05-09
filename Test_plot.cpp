// Built-in header files
#include <iostream>
#include <random>
#include <iostream>
#include <vector>

// Custom header files
#include "MonteCarlo.hpp"
#include "OptionData.hpp"
#include "FairValue.hpp"
#include "DataProcessing.hpp"

/*	DESCRIPTION	
	Runs the Euler and exact method for an option
	Plots the option's price, delta and gamma for a range of stock prices
	Returns an accurate price of an option
	Prints a summary of the results*/

int main()
{
	// Define variable
	double Smin, Smax, dS, K, T, r, sigma, D, alpha, accuracy;
	int style;
	long NT, M;
	char type;

	// Initialise variables
	// Option variables
	Smin = 10.0;		// Minimum stock price
	Smax = 100.0;		// Maximum stock price
	dS = 0.5;			// Stock price jump, 
						// dS > 0, otherwise the for-loop in MonteCarlo::generatePrices runs infinitely often
	K = 50.0;			// Strike price
	T = 1.0;			// Time to maturity in years
	r = 0.05;			// Constant interest rates
	sigma = 0.25;		// Constant volatility
	D = 0.01;			// Constant dividends
	type = 'P';			// Option type, 'C' = call, 'P' = Put
	style = 1;			// Option style, 0 = European, 1 = Arithmetic Asian, 2 = Geometric Asian
	NT = 100;			// Number of time steps 
	M = 1000;			// Number of Monte Carlo simulations

	// Accuracy variables (See Section 3.3 User defined accuracy in report)
	alpha = 0.05;		// Accuracy of confidence level 1 - alpha
	accuracy = 0.01;	// Desired accuracy of option price in dollars

	// Store option data
	OptionData OD(Smin, K, T, r, sigma, D, type, style);

	// Create two instances of Monte Carlo, exact and Euler method
	MonteCarlo MC_euler(OD, Smin, Smax, dS, NT, M, alpha, accuracy, 0, style);
	MonteCarlo MC_exact(OD, Smin, Smax, dS, NT, M, alpha, accuracy, 1, style);

	// Run the simulations
	std::cout << "Running the Euler method\n";
	MC_euler.run();
	std::cout << "Running the exact method\n";
	MC_exact.run();

	// Create tuple and store data
	std::tuple<MonteCarlo, MonteCarlo> MC_tuple{ MC_euler, MC_exact };
	DataProcessing DP(MC_tuple);

	// Plot price, delta and gamma
	DP.plotPrices();
	DP.plotDeltas();
	DP.plotGammas();
	
	// Prints the summary
	DP.printSummary();
	return 0;
}