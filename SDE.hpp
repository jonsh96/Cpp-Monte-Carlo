#ifndef SDE_HPP
#define SDE_HPP

#include "OptionData.hpp" // in local directory
#include <cmath>
#include <vector>
#include <iostream>

class SDE
{ // Defines drift + diffusion + data 
private:
	std::shared_ptr<OptionData> data;	// The data for the option
	int SDE_type;
	long NT;
	double v = 0.0;
public:
	SDE(const OptionData& optionData, int SDE_type, long NT) 
		: data(new OptionData(optionData)), SDE_type(SDE_type), NT(NT) 
	{
		v = data->r - data->D - 0.5 * data->sigma * data->sigma;
	}
	
	double drift(double t, double S);
	double diffusion(double t, double S);

	std::tuple<std::vector<double>, std::vector<double>> generatePaths(double S, const std::vector<double> &dW);
};

#endif // !SDE_HPP
