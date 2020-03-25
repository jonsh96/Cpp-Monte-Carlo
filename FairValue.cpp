#include "FairValue.hpp"

// Get functions
double FairValue::getPrice(double S) {return this->price->execute(S);}
double FairValue::getDelta(double S) {return this->delta->execute(S);}
double FairValue::getGamma(double S) {return this->gamma->execute(S);}
std::map<double, double> FairValue::getPriceMap() {return this->priceMap;}
std::map<double, double> FairValue::getDeltaMap() {return this->deltaMap;}
std::map<double, double> FairValue::getGammaMap() {return this->gammaMap;}

void FairValue::generateData(double Smin, double Smax, double dS)
{ 
	// Generates price, delta and gamma maps
	this->priceMap = generatePrices(Smin, Smax, dS);
	this->deltaMap = generateDeltas(Smin, Smax, dS);
	this->gammaMap = generateGammas(Smin, Smax, dS);
}

std::map<double, double> FairValue::generatePrices(double Smin, double Smax, double dS)
{
	// Generates price map
	std::map<double, double> myMap;
	for (double s = Smin; s < Smax; s += dS)
	{
		myMap.insert(std::pair<double, double>(s, getPrice(s)));
	}
	return myMap;
}

std::map<double, double> FairValue::generateDeltas(double Smin, double Smax, double dS)
{
	// Generates delta map
	std::map<double, double> myMap;
	for (double s = Smin; s < Smax; s += dS)
	{
		myMap.insert(std::pair<double, double>(s, getDelta(s)));
	}
	return myMap;
}

std::map<double, double> FairValue::generateGammas(double Smin, double Smax, double dS)
{
	// Generates gamma map
	std::map<double, double> myMap;
	for (double s = Smin; s < Smax; s += dS)
	{
		myMap.insert(std::pair<double, double>(s, getGamma(s)));
	}
	return myMap;
}


