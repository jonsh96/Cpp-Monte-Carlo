#include "MonteCarlo.hpp"
#include <map>
#include <string>

#ifndef DATA_PROCESSING_HPP
#define DATA_PROCESSING_HPP

// Stores two Monte Carlo simulations and processes the data to plot
class DataProcessing
{ 
private:
	std::tuple<MonteCarlo,MonteCarlo> MC;
public:
	// Constructor and destructor
	DataProcessing(const std::tuple<MonteCarlo, MonteCarlo> &tuple_MC) : MC(tuple_MC) {}
	~DataProcessing() {};

	// Data processing functions
	void writeToFile(const std::map<double, double>& myMap, const std::string& filename);
	void saveTitle();
	void storeData();

	// Plot functions
	void plotPaths();
	void plotPrices();
	void plotDeltas();
	void plotGammas();

	// Print functions
	void printSummary();
};

#endif // !DATA_PROCESSING_HPP
