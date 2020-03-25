#include "FDM.hpp"

// First-order centered difference
std::map<double, double> FDM::FOCD() const
{
	// Define and initialise variables 
	double delta, prev = 0.0, next = 0.0;
	double min = this->map_data.begin()->first;
	double max = this->map_data.rbegin()->first;
	double dS = (max - min) / static_cast<double>(this->map_data.size()-1.0);

	std::map<double, double> retMap;

	// Loop through price map (not including boundary elements) 
	for (double s = min + dS; s < max - dS; s += dS)
	{
		prev = this->map_data.at(s - dS);
		next = this->map_data.at(s + dS);

		// Delta (dC/dS) calculated using the centred finite-difference
		delta = (next - prev) / (2.0 * dS);
		retMap.insert(std::pair<double, double>(s, delta));
	}
	return retMap;
}

// Second-order centered difference
std::map<double, double> FDM::SOCD() const
{
	// Define and initialise variables 
	double gamma, prev = 0.0, next = 0.0, curr = 0.0;
	double min = this->map_data.begin()->first;
	double max = this->map_data.rbegin()->first;
	double dS = (max - min) / static_cast<double>(this->map_data.size() - 1.0);
	std::map<double, double> retMap;

	// Loop through price map (not including boundary elements)
	for (double s = min + dS; s < max - dS; s += dS)
	{
		prev = this->map_data.at(s - dS);
		curr = this->map_data.at(s);
		next = this->map_data.at(s + dS);

		// Gamma (dC^2/(dS)^2 calculated with the second-order central difference method
		gamma = (next - 2.0 * curr + prev) / (dS * dS);
		retMap.insert(std::pair<double, double>(s, gamma));
	}
	return retMap;
}