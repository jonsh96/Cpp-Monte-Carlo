#ifndef SDE_HPP
#define SDE_HPP

#include "OptionData.hpp" // in local directory
#include <cmath>
#include <iostream>

class SDE
{ // Defines drift + diffusion + data
private:
	std::shared_ptr<OptionData> data;	// The data for the option
public:
	SDE(const OptionData& optionData) : data(new OptionData(optionData)) {}
	
	double exact_drift(double t, double S);
	double exact_diffusion(double t, double S);

	double drift(double t, double S);
	double diffusion(double t, double S);
};

#endif // !SDE_HPP
