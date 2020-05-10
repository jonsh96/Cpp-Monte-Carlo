// Built-in header files
#include <iostream>
#include <random>
#include <iostream>
#include <vector>
#include <fstream>
#include <Windows.h>

// Custom header files
#include "MonteCarlo.hpp"
#include "OptionData.hpp"
#include "FairValue.hpp"
#include "DataProcessing.hpp"

/*	DESCRIPTION 
	- Performs M to M_max simulations (increasing x2 each time) 
		- For each method (Euler and exact)
		- For both call and put
		- For European, arithmetic Asian and geometric Asian options
	- Saves the values to a text file
	- Plots the time and error measurements and prints the complexity coefficients
	*/

int main()
{
	// Define variable
	double Smin, Smax, dS, K, T, r, sigma, D, alpha, accuracy;
	int style;
	long NT, M, M_max;
	char type[2] = { 'C','P' };	// Call and put
	std::string filename;

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
	//type = 'P';		// Option type, 'C' = call, 'P' = Put
	//style = 0;		// Option style, 0 = European, 1 = Arithmetic Asian, 2 = Geometric Asian
						// Here we observe styles 0 and 2 since the approximation of the Arithmetic wasn't very goog
	NT = 100;			// Number of time steps
	M = 10;				// Number of Monte Carlo simulations
	M_max = 100'000;	// Here the number of simulations starts at M and goes up until 100,000

	// Accuracy variables (See Section 3.3 User defined accuracy in report)
	alpha = 0.05;		// Accuracy of confidence level 1 - alpha
	accuracy = 0.01;	// Desired accuracy of option price in dollars

	for (int j = 0; j <= 1; j++)
	{
		for (style = 0; style <= 2; style += 1)
		{
			// Printing option type
			std::string option_str = "";
			switch (style)
			{
			case 0:
				option_str += "European ";
				break;
			case 1:
				option_str += "Arithmetic Asian ";
				break;
			case 2:
				option_str += "Geometric Asian ";
				break;
			}
			switch (j)
			{
			case 0:
				option_str += "call option";
				break;
			case 1:
				option_str += "put option";
				break;
			}
			std::cout << option_str << "\n";

			// Store option data
			OptionData OD(Smin, K, T, r, sigma, D, type[j], style);

			// Create two instances of Monte Carlo, exact and Euler method
			MonteCarlo MC_euler(OD, Smin, Smax, dS, NT, M, alpha, accuracy, 0, style);
			MonteCarlo MC_exact(OD, Smin, Smax, dS, NT, M, alpha, accuracy, 1, style);

			// Store them as a tuple
			std::tuple<MonteCarlo, MonteCarlo> MC_tuple{ MC_euler, MC_exact };

			// Measure time and accuracy for 10, 20, 40, 80, ...
			std::vector<std::pair<double, double>> euler_meas;
			std::vector<std::pair<double, double>> exact_meas;

			for (int i = M; i < M_max; i *= 2)
			{
				std::cout << "Number of simulations: " << i << "\n";
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

			// Creates filename
			std::ofstream myFile;
			if(j == 0)
				filename = "data/call_style_" + std::to_string(style) + "_measurements.txt";
			else
				filename = "data/put_style_" + std::to_string(style) + "_measurements.txt";

			// Writes measurements into file
			myFile.open(filename);
			for (int i = 0; i < exact_meas.size(); i++)
			{
				myFile << M * pow(2, i) << "," << euler_meas[i].first << "," << euler_meas[i].second << ",";
				myFile << exact_meas[i].first << "," << exact_meas[i].second << "\n";
			}
			// Closes file
			myFile.close();
			std::cout << "\n";
		}
	}

	// Plot paths using the system command to execute python file
	std::string command = "plot\\time_error_analysis.py";
	system(command.c_str());
	return 0;
}