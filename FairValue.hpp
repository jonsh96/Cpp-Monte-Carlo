#ifndef FAIR_VALUE_HPP
#define FAIR_VALUE_HPP

#include "OptionCommand.hpp"
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <windows.h>

class FairValue
{
private:
	// Option parameters
	double K, r, T, D, sigma, Smin, Smax, dS;
	char type; 
	int style; 
	
	// Option command instances to get fair price
	OptionCommand *price, *delta, *gamma;

	// Map with stock price as a key and the price/delta/gamma as value
	std::map<double, double> priceMap, deltaMap, gammaMap;

public:
	// Constructors and destructors
	FairValue() : style(0), K(0.0), r(0.0), T(0.0), D(0.0), 
		sigma(0.0), type('C'), Smin(0.0), Smax(0.0), dS(0.0)
	{	// Assigning price, delta, gamma pointers
		this->price = new CallPrice(K, T, r, D, sigma);
		this->delta = new CallDelta(K, T, r, D, sigma);
		this->gamma = new CallGamma(K, T, r, D, sigma);
	}
	FairValue(const FairValue& fv) : K(fv.K), r(fv.r), T(fv.T), D(fv.D), sigma(fv.sigma), 
		Smin(fv.Smin), Smax(fv.Smax), dS(fv.dS), type(fv.type), style(fv.style) 
	{	// Assigning price, delta, gamma pointers	
		if (type == 'C')
		{
			if (this->style == 0)
			{
				this->price = new CallPrice(K, T, r, D, sigma);
				this->delta = new CallDelta(K, T, r, D, sigma);
				this->gamma = new CallGamma(K, T, r, D, sigma);
			}
			else if (this->style == 1)
			{
				this->price = new ArithmeticAsianCallPrice(K, T, r, D, sigma);
				this->delta = new ArithmeticAsianCallDelta(K, T, r, D, sigma);
				this->gamma = new ArithmeticAsianCallGamma(K, T, r, D, sigma);
			}
			else if (this->style == 2)
			{
				this->price = new GeometricAsianCallPrice(K, T, r, D, sigma);
				this->delta = new GeometricAsianCallDelta(K, T, r, D, sigma);
				this->gamma = new GeometricAsianCallGamma(K, T, r, D, sigma);
			}
		}
		else
		{
			if (this->style == 0)
			{
				this->price = new PutPrice(K, T, r, D, sigma);
				this->delta = new PutDelta(K, T, r, D, sigma);
				this->gamma = new PutGamma(K, T, r, D, sigma);
			}
			else if (this->style == 1)
			{
				this->price = new ArithmeticAsianPutPrice(K, T, r, D, sigma);
				this->delta = new ArithmeticAsianPutDelta(K, T, r, D, sigma);
				this->gamma = new ArithmeticAsianPutGamma(K, T, r, D, sigma);
			}
			else if (this->style == 2)
			{
				this->price = new GeometricAsianPutPrice(K, T, r, D, sigma);
				this->delta = new GeometricAsianPutDelta(K, T, r, D, sigma);
				this->gamma = new GeometricAsianPutGamma(K, T, r, D, sigma);
			}
		}
		generateData(Smin, Smax, dS);
	}
	FairValue(int style, double K, double r, double T, double D, 
		double sigma, char type, double Smin, double Smax, double dS) :
		style(style), K(K), r(r), T(T), D(D), sigma(sigma), type(type),
		Smin(Smin), Smax(Smax), dS(dS)
	{	// Assigning price, delta, gamma pointers	
		if (type == 'C')
		{
			if (this->style == 0)
			{
				this->price = new CallPrice(K, T, r, D, sigma);
				this->delta = new CallDelta(K, T, r, D, sigma);
				this->gamma = new CallGamma(K, T, r, D, sigma);
			}
			else if (this->style == 1)
			{
				this->price = new ArithmeticAsianCallPrice(K, T, r, D, sigma);
				this->delta = new ArithmeticAsianCallDelta(K, T, r, D, sigma);
				this->gamma = new ArithmeticAsianCallGamma(K, T, r, D, sigma);
			}
			else if (this->style == 2)
			{
				this->price = new GeometricAsianCallPrice(K, T, r, D, sigma);
				this->delta = new GeometricAsianCallDelta(K, T, r, D, sigma);
				this->gamma = new GeometricAsianCallGamma(K, T, r, D, sigma);
			}
		}
		else
		{
			if (this->style == 0)
			{
				this->price = new PutPrice(K, T, r, D, sigma);
				this->delta = new PutDelta(K, T, r, D, sigma);
				this->gamma = new PutGamma(K, T, r, D, sigma);
			}
			else if (this->style == 1)
			{
				this->price = new ArithmeticAsianPutPrice(K, T, r, D, sigma);
				this->delta = new ArithmeticAsianPutDelta(K, T, r, D, sigma);
				this->gamma = new ArithmeticAsianPutGamma(K, T, r, D, sigma);
			}
			else if (this->style == 2)
			{
				this->price = new GeometricAsianPutPrice(K, T, r, D, sigma);
				this->delta = new GeometricAsianPutDelta(K, T, r, D, sigma);
				this->gamma = new GeometricAsianPutGamma(K, T, r, D, sigma);
			}
		}

		generateData(Smin, Smax, dS);
	}
	~FairValue(){}

	// Get functions
	double getPrice(double S);
	double getDelta(double S);
	double getGamma(double S);
	std::map<double, double> getPriceMap();
	std::map<double, double> getDeltaMap();
	std::map<double, double> getGammaMap();

	// Calculations
	void generateData(double Smin, double Smax, double dS);
	std::map<double, double> generatePrices(double Smin, double Smax, double dS);
	std::map<double, double> generateDeltas(double Smin, double Smax, double dS);
	std::map<double, double> generateGammas(double Smin, double Smax, double dS);
};

#endif // ! FAIR_VALUE_HPP