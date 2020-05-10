#include "MonteCarlo.hpp"
#include <map>
#include <string>

#ifndef DATA_PROCESSING_HPP
#define DATA_PROCESSING_HPP

/*	ABOUT
	- Stores two Monte Carlo simulations 
	- Processes and plots the data
*/
class DataProcessing
{ 
private:
	std::tuple<MonteCarlo,MonteCarlo> MC;
public:
	// Constructor and destructor
	DataProcessing(const std::tuple<MonteCarlo, MonteCarlo> &tuple_MC) : MC(tuple_MC) 
	{
		storeData();
	}
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
