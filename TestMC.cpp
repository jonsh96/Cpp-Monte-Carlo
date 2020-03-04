#include <iostream>
#include "MonteCarlo.hpp"
#include "OptionData.hpp"
#include "BlackScholes.hpp"

// TODO: COMMENT
// TODO: ADD MORE INPUTS

int main()
{
	/*double S0 = 55.0, K = 65.0, T = 1.0, r = 0.03, sigma = 0.2, D = 0.01;
	char type = 'C';
	bool van = true;
	long NT = 250, M = 1000;*/
	
	double Smin, Smax, dS, K, T, r, sigma, D;
	char type;
	bool van;
	long NT, M;
	
	std::cout << "Input the following parameters:\n";
	std::cout << "Minimum price:\t\t";
	std::cin >> Smin;
	std::cout << "Maximum price:\t\t";
	std::cin >> Smax;
	std::cout << "Price jumps:\t\t";
	std::cin >> dS;
	std::cout << "Strike price:\t\t";
	std::cin >> K;
	std::cout << "Time to maturity:\t";
	std::cin >> T;
	std::cout << "Interest rate:\t\t";
	std::cin >> r;
	std::cout << "Volatility:\t\t";
	std::cin >> sigma;
	std::cout << "Dividend yield:\t\t";
	std::cin >> D;
	std::cout << "Call or put [C/P]:\t";
	std::cin >> type;
	std::cout << "Asian option [Y/N]:\t";
	char ans; 
	std::cin >> ans;
	if (ans == 'Y' || ans == 'y')
		van = false;
	else
		van = true;

	std::cout << "Number of time steps:\t";
	std::cin >> NT;
	std::cout << "Number of simulations:\t";
	std::cin >> M;
	OptionData OD(Smin, K, T, r, sigma, D, type, van);
	MonteCarlo MC(OD, Smin, Smax, dS, NT, M);
	MC.printSummary();
	std::cout << "Plot prices? [Y/N]:\t";
	std::cin >> ans;
	if (ans == 'Y' || ans == 'y')
	{
		MC.storeData();
		MC.plotPrices();
	}
	std::cout << "Plot deltas? [Y/N]:\t";
	std::cin >> ans;
	if (ans == 'Y' || ans == 'y')
	{
		MC.plotDeltas();
	}
	std::cout << "Plot gammas? [Y/N]:\t";
	std::cin >> ans;
	if (ans == 'Y' || ans == 'y')
	{
		MC.plotGammas();
	}
	std::cout << "Rerun to get more accurate results? [Y/N]: ";
	std::cin >> ans;
	if (ans == 'Y' || ans == 'y')
	{
		MC.setNumberOfSimulations(MC.minSimulationsNeeded());
		MC.refresh();
		MC.generateData();
		MC.storeData();
		MC.printSummary();
		std::cout << "Plot prices? [Y/N]: ";
		std::cin >> ans;
		if (ans == 'Y' || ans == 'y')
		{
			MC.storeData();
			MC.plotPrices();
		}
		std::cout << "Plot deltas? [Y/N]: ";
		std::cin >> ans;
		if (ans == 'Y' || ans == 'y')
		{
			MC.plotDeltas();
		}
		std::cout << "Plot gammas? [Y/N]: ";
		std::cin >> ans;
		if (ans == 'Y' || ans == 'y')
		{
			MC.plotGammas();
		}
	}
	return 0;
}