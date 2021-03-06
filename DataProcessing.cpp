#include "DataProcessing.hpp"
#include <fstream>

void DataProcessing::writeToFile(const std::map<double, double> &myMap, const std::string &filename)
{
	// Writes map to text file using <fstream>
	std::ofstream myFile;
	myFile.open(filename);
	for (auto it = myMap.begin(); it != myMap.end(); it++)
	{
		myFile << it->first << ", " << it->second << "\n";
	}
	myFile.close();
}

void DataProcessing::saveTitle()
{
	// Writing the title for the plots
	std::ofstream title_file;
	title_file.open("data/title.txt");
	title_file << std::get<0>(this->MC); // Using overloaded << operator 
	title_file.close();
}
 
void DataProcessing::storeData()
{
	// Save title of figures
	saveTitle();

	// Fair value price, delta, gamma
	this->writeToFile(std::get<0>(this->MC).getFairOption().getPriceMap(), "data/option_price.txt");
	this->writeToFile(std::get<0>(this->MC).getFairOption().getDeltaMap(), "data/option_delta.txt");
	this->writeToFile(std::get<0>(this->MC).getFairOption().getGammaMap(), "data/option_gamma.txt");

	// Write the maps (prices, deltas, gammas) to text file to plot data in Python
	if (std::get<0>(this->MC).getSDEtype() == 0) 
	{	// Euler method
		this->writeToFile(std::get<0>(this->MC).getPrices(), "data/MC_prices.txt");
		this->writeToFile(std::get<0>(this->MC).getDeltas(), "data/MC_deltas.txt");
		this->writeToFile(std::get<0>(this->MC).getGammas(), "data/MC_gammas.txt");
		this->writeToFile(std::get<0>(this->MC).getStdDev(), "data/MC_stddev.txt");
	}
	else
	{	// Exact method
		this->writeToFile(std::get<0>(this->MC).getPrices(), "data/MC_exact_prices.txt");
		this->writeToFile(std::get<0>(this->MC).getDeltas(), "data/MC_exact_deltas.txt");
		this->writeToFile(std::get<0>(this->MC).getGammas(), "data/MC_exact_gammas.txt");
		this->writeToFile(std::get<0>(this->MC).getStdDev(), "data/MC_exact_stddev.txt");
	}

	// Write the maps (prices, deltas, gammas) to text file to plot data in Python
	if (std::get<1>(this->MC).getSDEtype() == 0)
	{	// Euler method
		this->writeToFile(std::get<1>(this->MC).getPrices(), "data/MC_prices.txt");
		this->writeToFile(std::get<1>(this->MC).getDeltas(), "data/MC_deltas.txt");
		this->writeToFile(std::get<1>(this->MC).getGammas(), "data/MC_gammas.txt");
		this->writeToFile(std::get<1>(this->MC).getStdDev(), "data/MC_stddev.txt");
	}
	else
	{	// Exact method
		this->writeToFile(std::get<1>(this->MC).getPrices(), "data/MC_exact_prices.txt");
		this->writeToFile(std::get<1>(this->MC).getDeltas(), "data/MC_exact_deltas.txt");
		this->writeToFile(std::get<1>(this->MC).getGammas(), "data/MC_exact_gammas.txt");
		this->writeToFile(std::get<1>(this->MC).getStdDev(), "data/MC_exact_stddev.txt");
	}
}

void DataProcessing::plotPaths()
{
	// Plot paths using the system command to execute python file
	std::string command = "plot\\plot_test.py";
	system(command.c_str());
}

void DataProcessing::plotPrices()
{
	// Plot price using the system command to execute python file
	std::string command = "plot\\plot_price.py";
	system(command.c_str());
}

void DataProcessing::plotDeltas()
{
	// Plot deltas using the system command to execute python file
	std::string command = "plot\\plot_delta.py";
	system(command.c_str());
}

void DataProcessing::plotGammas()
{
	// Plot gammas using the system command to execute python file
	std::string command = "plot\\plot_gamma.py";
	system(command.c_str());
}

void DataProcessing::printSummary()
{
	// Prints the summary of the Monte Carlo simulation
	MonteCarlo MC_euler = std::get<0>(MC);
	MonteCarlo MC_exact = std::get<1>(MC);

	std::cout << "----------------------------------";
	std::cout << "\nSummary of Monte Carlo simulation:";
	std::cout << "\n----------------------------------";
	std::cout << "\nEuler method:";
	std::cout << "\nMax pricing error:\t" << MC_euler.maxPricingError();
	std::cout << "\nMax standard error:\t" << MC_euler.maxStandardError();
	std::cout << "\nMax standard deviation:\t" << MC_euler.maxStandardDeviation();
	std::cout << "\nMin simulations needed:\t" << MC_euler.minSimulationsNeeded();
	std::cout << "\nTime elapsed:\t\t" << MC_euler.getTimeElapsed();
	std::cout << "\n----------------------------------";
	std::cout << "\nExact method:";
	std::cout << "\nMax pricing error:\t" << MC_exact.maxPricingError();
	std::cout << "\nMax standard error:\t" << MC_exact.maxStandardError();
	std::cout << "\nMax standard deviation:\t" << MC_exact.maxStandardDeviation();
	std::cout << "\nMin simulations needed:\t" << MC_exact.minSimulationsNeeded();
	std::cout << "\nTime elapsed:\t\t" << MC_exact.getTimeElapsed();
	std::cout << "\n----------------------------------\n";
}