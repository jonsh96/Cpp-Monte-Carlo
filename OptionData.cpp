#include "OptionData.hpp"
#include <cmath>

// Set functions
void OptionData::setInitialPrice(double S) {this->S0 = S;}
void OptionData::setStrike(double K) {this->K = K; }
void OptionData::setMaturity(double T) {this->T = T; }
void OptionData::setInterestRate(double r) {this->r = r;}
void OptionData::setVolatility(double sigma) {this->sigma = sigma;}
void OptionData::setDividend(double D) {this->D = D;}
void OptionData::setType(char type) {this->type = type;}
void OptionData::setOptionType(int style) {this->style = style;}
 
// Get functions
double OptionData::getInitialPrice() {return this->S0;}
double OptionData::getStrike() {return this->K;}
double OptionData::getMaturity() {return this->T;}
double OptionData::getInterestRate() {return this->r;}
double OptionData::getVolatility() {return this->sigma;}
double OptionData::getDividend() {return this->D;}
char OptionData::getType() {return this->type;}
int OptionData::getOptionType() {return this->style;}

// Ostream overload
std::ostream& operator<<(std::ostream& os, const OptionData& op)
{
	os << op.T << ", " << op.r << ", " << op.D << ", " << op.sigma;
	return os;
}

double OptionData::payoff(std::vector<double> path)
{ 
	// Payoff function
	// TODO: Barrier options
	// - tuple, min, max, 0 by default
	// - knock in, out, etc ...

	double S = 0.0, P;
	if (style == 0)		// European
		S = path.back();
	else if(style == 1)
		S = std::accumulate(path.begin(), path.end(), 0) / static_cast<double>(path.size());
	else if(style == 2) // Asian
	{
		double geo_sum = path[0];
		for (int i = 1; i < path.size(); i++)
		{
			geo_sum *= path[i];
		}
		S = pow(geo_sum, 1.0 / static_cast<double>(path.size()));
	}

	/* TODO: Add barrier option functionality
		- Implement the four main types
			- Up-and-out
			- Down-and-out
			- Up-and-in
			- Down-and-in
		- By defining a boolean type and using
			auto it = max_element(std::begin(path), std::end(path)); // c++11
			double max_price = *it;

			and/or 

			auto it = min_element(std::begin(cloud), std::end(cloud)); // c++11
			double min_price = *it;

	else if (style == 4) // Barrier
	{
		// ...
	}

		- The boolean should then be used in this if-loop to determine whether 
		  or not the option has a payoff

	bool barrierHit = false;
	*/
	if (type == 'C' || type == 'c')	
		P = std::max(S - this->K, 0.0); // CALL
	else	
		P = std::max(this->K - S, 0.0); // PUT

	return P;
}