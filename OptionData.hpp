// OptionData.hpp
//
// Encapsulate option data.
//
// (C) Datasim Education BV 2008-2016

// TODO: GO OVER + COMMENT + ADD FUNCTIONS IF NEEDED
#ifndef OptionData_HPP
#define OptionData_HPP

#include <algorithm> // for max()
#include <boost/parameter.hpp>
#include <numeric>
#include <vector>
#include <iostream>

namespace OptionParams
{
	BOOST_PARAMETER_KEYWORD(Tag, initalPrice)
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
	double S0, K, T, r, sigma, D;
	char type;		// type == 'C' if call, type == 'P' if put
	bool vanilla;	// vanilla == false if Asian option, otherwise true
	
	// Default constructor
	explicit constexpr OptionData() : S0(0.0), K(0.0), T(0.0), r(0.0), 
		sigma(0.0), D(0.0), type('C'), vanilla(true) {}

	// Copy constructor
	explicit constexpr OptionData(const OptionData &opt) : S0(opt.S0),  K(opt.K), 
		T(opt.T), r(opt.r), sigma(opt.sigma), D(opt.D), type(opt.type), vanilla(opt.vanilla) {}

	// Constructor
	explicit constexpr OptionData(double initialPrice, double strike, double expiration, double interestRate,
		double volatility, double dividend, char PC, bool van) : S0(initialPrice), K(strike), T(expiration), 
		r(interestRate), sigma(volatility), D(dividend), type(PC), vanilla(van) {}

	template <typename ArgPack> OptionData(const ArgPack& args)
	{
		S0 = args[OptionParams::initalPrice];
		K = args[OptionParams::strike];
		T = args[OptionParams::expiration];
		r = args[OptionParams::interestRate];
		sigma = args[OptionParams::volatility];
		D = args[OptionParams::dividend];
		type = args[OptionParams::optionType];
		vanilla = args[OptionParams::vanilla];
	}

	// SET FUNCTIONS
	void setInitialPrice(double S);
	void setStrike(double K);
	void setMaturity(double T);
	void setInterestRate(double r);
	void setVolatility(double sigma);
	void setDividend(double D);
	void setType(char type);
	void setOptionType(bool van);

	// GET FUNCTIONS
	double getInitialPrice();
	double getStrike();
	double getMaturity();
	double getInterestRate();
	double getVolatility();
	double getDividend();
	char getType();
	bool getOptionType();

	// CALCULATIONS
	double payoff(std::vector<double> path);

	// OPERATORS
	friend std::ostream & operator<<(std::ostream& os, const OptionData& op);
};

#endif
