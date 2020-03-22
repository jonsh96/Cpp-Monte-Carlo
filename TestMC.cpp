#include <iostream>
#include "MonteCarlo.hpp"
#include "OptionData.hpp"
#include "FairValue.hpp"
#include "DataProcessing.hpp"

#include <random>
#include <iostream>
#include <vector>

int main()
{
	// Define variable
	double Smin, Smax, dS, K, T, r, sigma, D, alpha, accuracy;
	char type;
	int style;
	long NT, M;
	
	// Initialise variables
	Smin = 1.0;
	Smax = 20.0;
	dS = 0.5;
	K = 10.0;
	T = 1.0;
	r = 0.04; 
	sigma = 0.2; 
	D = 0.0;
	alpha = 0.05; 
	accuracy = 0.01;
	type = 'C';
	style = 0;
	NT = 100;
	M = 5000;

	// Store option data
	OptionData OD(Smin, K, T, r, sigma, D, type, style);
	
	// Create two instances of Monte Carlo, exact and Euler method
	MonteCarlo MC_exact(OD, Smin, Smax, dS, NT, M, alpha, accuracy, 0, style);
	MonteCarlo MC_euler(OD, Smin, Smax, dS, NT, M, alpha, accuracy, 1, style);
	
	// Store them as a tuple
	std::tuple<MonteCarlo, MonteCarlo> MC_tuple{ MC_exact,MC_euler };
	std::cout << std::get<0>(MC_tuple) << std::endl;
	
	// Run both simulations
	std::get<0>(MC_tuple).run();
	std::get<1>(MC_tuple).run();	
	char ans;
	std::cout << "Plot results? [Y/N]\t\t";
	std::cin >> ans;
	if (ans == 'Y' || ans == 'y')
	{
		DataProcessing DP = DataProcessing(MC_tuple);
		DP.saveTitle();
		DP.storeData();
		DP.plotPrices();
		DP.plotDeltas();
		DP.plotGammas();
	}

	return 0;
}