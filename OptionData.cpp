#include "OptionData.hpp"
// TODO: COMMENT 

// SET FUNCTIONS
void OptionData::setInitialPrice(double S)
{
	this->S0 = S;
}

void OptionData::setStrike(double K)
{
	this->K = K;
}

void OptionData::setMaturity(double T)
{
	this->T = T;
}

void OptionData::setInterestRate(double r)
{
	this->r = r;
}
void OptionData::setVolatility(double sigma)
{
	this->sigma = sigma;
}

void OptionData::setDividend(double D)
{
	this->D = D;
}
void OptionData::setType(char type)
{
	this->type = type;
}

void OptionData::setOptionType(bool van)
{
	this->vanilla = van;
}

// GET FUNCTIONS
double OptionData::getInitialPrice()
{
	return this->S0;
}

double OptionData::getStrike()
{
	return this->K;
}

double OptionData::getMaturity()
{
	return this->T;
}
double OptionData::getInterestRate()
{
	return this->r;
}

double OptionData::getVolatility()
{
	return this->sigma;
}

double OptionData::getDividend()
{
	return this->D;
}

char OptionData::getType()
{
	return this->type;
}

bool OptionData::getOptionType()
{
	return this->vanilla;
}

std::ostream& operator<<(std::ostream& os, const OptionData& op)
{
	os << ", " << op.S0 << ", " << op.T << ", " << op.r << ", " << op.D << ", " << op.sigma << ", ";
	return os;
}

double OptionData::payoff(std::vector<double> path)
{ 
	// Payoff function
	double S, P;
	if (vanilla) // European
		S = path.back();
	else // Asian
		S = std::accumulate(path.begin(), path.end(), 0) / static_cast<double>(path.size());
	
	if (type == 'C' || type == 'c')	
		P = std::max(S - this->K, 0.0); // CALL
	else	
		P = std::max(this->K - S, 0.0); // Put

	return P;
}