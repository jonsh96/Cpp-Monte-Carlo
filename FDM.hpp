// Built-in header files
#include <map>

#ifndef FDM_HPP
#define FDM_HPP

/*	ABOUT
	- Finite difference methods used to calculate greeks
	- Only needs a map with stock price as key and option price as a valuee*/

class FDM
{
private: 
	std::map<double, double> map_data;
public:
	FDM(const std::map<double, double>& map) : map_data(map) {};
	~FDM() {};

	// First-order centered difference
	std::map<double, double> FOCD() const;

	// Second-order centered difference
	std::map<double, double> SOCD() const;
};

#endif // !FDM_HPP
