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
	Smin = 10.0;
	Smax = 50.0;
	dS = 0.5;
	K = 25.0;
	T = 1.0;
	r = 0.03; 
	sigma = 0.2; 
	D = 0.0;
	alpha = 0.05; 
	accuracy = 0.01;
	type = 'C';
	style = 2;
	NT = 100;
	M = 10;

	// Store option data
	OptionData OD(Smin, K, T, r, sigma, D, type, style); 
	
	// Create two instances of Monte Carlo, exact and Euler method
	MonteCarlo MC_euler(OD, Smin, Smax, dS, NT, M, alpha, accuracy, 0, style);
	MonteCarlo MC_exact(OD, Smin, Smax, dS, NT, M, alpha, accuracy, 1, style);
	
	// Store them as a tuple
	std::tuple<MonteCarlo, MonteCarlo> MC_tuple{MC_euler, MC_exact};
	
	// Measure time and accuracy for 10, 100, 1000, 10000, 100000
	std::vector<std::pair<double, double>> euler_meas;
	std::vector<std::pair<double, double>> exact_meas;

	for (int i = M; i < 100000; i *= 2)
	{
		std::get<0>(MC_tuple).setNumberOfSimulations(i);
		std::get<1>(MC_tuple).setNumberOfSimulations(i);
		// Run both simulations
		std::get<0>(MC_tuple).rerun();
		std::get<1>(MC_tuple).rerun();
		// Extract time and accuracy
		euler_meas.push_back(std::pair<double, double>
			(std::get<0>(MC_tuple).getTimeElapsed(), std::get<0>(MC_tuple).maxPricingError()));
		exact_meas.push_back(std::pair<double, double>
			(std::get<1>(MC_tuple).getTimeElapsed(), std::get<1>(MC_tuple).maxPricingError()));
	}

	std::cout << std::get<0>(MC_tuple) << std::endl;
	std::cout << "\nN\tExact\t\tEuler\n";

	for (int i = 0; i < exact_meas.size(); i++)
	{
		std::cout << 10*pow(2,i) << "," << exact_meas[i].first << "," << exact_meas[i].second << ",";
		std::cout << euler_meas[i].first << "," << euler_meas[i].second << "\n";
	}

	DataProcessing DP = DataProcessing(MC_tuple);
	DP.printSummary();
	DP.saveTitle();
	DP.storeData();
	DP.plotPrices();
	DP.plotDeltas();
	DP.plotGammas();


	/*
	// Run both simulations
	std::get<0>(MC_tuple).run();
	std::get<1>(MC_tuple).run();	

	// Offer to plot using MatPlotLib and Numpy in Python
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
	*/
	return 0;
}