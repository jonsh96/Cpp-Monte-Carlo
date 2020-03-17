#include <iostream>
#include "MonteCarlo.hpp"
#include "OptionData.hpp"
#include "BlackScholes.hpp"

// TODO: COMMENT
// TODO: ADD MORE INPUTS

int main()
{
	double Smin, Smax, dS, K, T, r, sigma, D, percentile, dollarAccuracy;
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
	std::cout << "Desired accuracy [%]:\t";
	std::cin >> percentile;
	percentile /= 100;
	std::cout << "Desired accuracy [$]:\t";
	std::cin >> dollarAccuracy;

	OptionData OD(Smin, K, T, r, sigma, D, type, van);
	MonteCarlo MC_exact(OD, Smin, Smax, dS, NT, M, percentile, dollarAccuracy, true);
	MonteCarlo MC_euler(OD, Smin, Smax, dS, NT, M, percentile, dollarAccuracy, false);
	std::cout << "\nExact summary:\n";
	MC_exact.printSummary();
	std::cout << "Euler summary:\n";
	MC_euler.printSummary();

	std::cout << "\nPlot prices? [Y/N]:\t";
	std::cin >> ans;
	if (ans == 'Y' || ans == 'y')
	{
		MC_exact.storeData();
		MC_euler.storeData();
		MC_exact.saveTitle();
		MC_exact.plotPrices();
	}
	std::cout << "Plot deltas? [Y/N]:\t";
	std::cin >> ans;
	if (ans == 'Y' || ans == 'y')
	{
		MC_exact.plotDeltas();
	}
	std::cout << "Plot gammas? [Y/N]:\t";
	std::cin >> ans;
	if (ans == 'Y' || ans == 'y')
	{
		MC_exact.plotGammas();
	}
	std::cout << "Rerun to get more accurate results? [Y/N]: ";
	std::cin >> ans;
	if (ans == 'Y' || ans == 'y')
	{
		MC_exact.setNumberOfSimulations(MC_exact.minSimulationsNeeded());
		MC_exact.refresh();
		MC_exact.generateData();
		MC_exact.storeData();
		MC_exact.saveTitle();

		MC_euler.setNumberOfSimulations(MC_euler.minSimulationsNeeded());
		MC_euler.refresh();
		MC_euler.generateData();
		MC_euler.storeData();

		std::cout << "\nExact summary:\n";
		MC_exact.printSummary();
		std::cout << "Euler summary:\n";
		MC_euler.printSummary();

		std::cout << "\nPlot prices? [Y/N]:\t";
		std::cin >> ans;
		if (ans == 'Y' || ans == 'y')
		{
			MC_exact.storeData();
			MC_euler.storeData();
			MC_exact.plotPrices();
		}
		std::cout << "Plot deltas? [Y/N]:\t";
		std::cin >> ans;
		if (ans == 'Y' || ans == 'y')
		{
			MC_exact.plotDeltas();
		}
		std::cout << "Plot gammas? [Y/N]:\t";
		std::cin >> ans;
		if (ans == 'Y' || ans == 'y')
		{
			MC_exact.plotGammas();
		}
	}
	bool isOver = false;
	char price_ans;
	double input_price;

	while (!isOver)
	{
		std::cout << "\nWould you like to get the price of the option? [Y/N]: ";
		std::cin >> price_ans;
		if (price_ans == 'Y' || price_ans == 'y')
		{
			std::cout << "\nStock price: ";
			std::cin >> input_price;
			MC_exact.setInitialPrice(input_price);
			MC_euler.setInitialPrice(input_price);
			MC_exact.generatePaths(input_price);
			MC_euler.generatePaths(input_price);
			std::cout << "Exact: \t" << MC_exact.getOptionPrice() << "\n";
			std::cout << "Euler: \t" << MC_euler.getOptionPrice() << "\n";
		}
		else
		{
			isOver = true;
		}
	}
	return 0;
}