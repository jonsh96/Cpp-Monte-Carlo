#include "OptionData.hpp" // in local directory
#include <cmath>
#include <iostream>

#ifndef SDE_HPP
#define SDE_HPP

class SDE
{ // Defines drift + diffusion + data
private:
	std::shared_ptr<OptionData> data;	// The data for the option
public:
	SDE(const OptionData& optionData) : data(new OptionData(optionData)) {}

	double drift(double t, double S)
	{ // Drift term	
		return (data->r - data->D) * S; // r - D
	}

	double diffusion(double t, double S)
	{ // Diffusion term
		return data->sig * S;
	}
};

#endif // !SDE_HPP
