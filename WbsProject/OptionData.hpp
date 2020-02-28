// OptionData.hpp
//
// Encapsulate option data.
//
// (C) Datasim Education BV 2008-2016

#ifndef OptionData_HPP
#define OptionData_HPP

#include <algorithm> // for max()
#include <boost/parameter.hpp>
#include <numeric>
#include <vector>

namespace OptionParams
{
	BOOST_PARAMETER_KEYWORD(Tag, strike)
	BOOST_PARAMETER_KEYWORD(Tag, expiration)
	BOOST_PARAMETER_KEYWORD(Tag, interestRate)
	BOOST_PARAMETER_KEYWORD(Tag, volatility)
	BOOST_PARAMETER_KEYWORD(Tag, dividend)
	BOOST_PARAMETER_KEYWORD(Tag, optionType)
	BOOST_PARAMETER_KEYWORD(Tag, vanilla)
}

// Encapsulate all data in one place
struct OptionData
{ 
	// Option data + behaviour
	double K;
	double T;
	double r;
	double sig;

	// Extra data 
	double D;		// dividend
	int type;		// type == 1 if call, type == -1 if put
	bool vanilla;	// vanilla == false if Asian option, otherwise true
	
	// Default constructor
	explicit constexpr OptionData() : K(0.0), T(0.0),
		r(0.0), sig(0.0), D(0.0), type(0), vanilla(true) {}

	// Copy constructor
	explicit constexpr OptionData(const OptionData &opt) : K(opt.K), T(opt.T), 
		r(opt.r), sig(opt.sig), D(opt.D), type(opt.type), vanilla(opt.vanilla) {}

	// Constructor
	explicit constexpr OptionData(double strike, double expiration, double interestRate,
		double volatility, double dividend, int PC, bool van)
		: K(strike), T(expiration), r(interestRate), sig(volatility), D(dividend), type(PC), vanilla(van)
	{}

	/* ASK DUFFY?
	template <typename ArgPack> OptionData(const ArgPack& args)
	{
		K = args[OptionParams::strike];
		T = args[OptionParams::expiration];
		r = args[OptionParams::interestRate];
		sig = args[OptionParams::volatility];
		D = args[OptionParams::dividend];
		type = args[OptionParams::optionType];
		vanilla = args[OptionParams::vanilla];

		//std::cout << "K " << K << ", T " << T << ",r " << r << std::endl;
		//std::cout << "vol " << sig << ", div " << D << ",type " << type << std::endl;
	}*/

	double payoff(std::vector<double> path)
	{ // Payoff function
		double S;
		if (vanilla)
			S = path[path.size()];
		else
			S = std::accumulate(path.begin(), path.end(), 0) / path.size();;

		if (type == 1)
		{ 
			// Call
			return std::max(S - K, 0.0);
		}
		else if (type == 0)
		{	
			// Put
			return std::max(K - S, 0.0);
		}
	}
};

#endif