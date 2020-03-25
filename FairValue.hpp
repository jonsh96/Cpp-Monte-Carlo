#ifndef FAIR_VALUE_HPP
#define FAIR_VALUE_HPP

#include "OptionCommand.hpp"
#include "OptionData.hpp"
#include <vector>
#include <map>
#include <string>
#include <fstream> 
#include <windows.h>

class FairValue
{
private:
	// Option parameters
	OptionData data;		// The data for the option
	double Smin, Smax, dS;

	// Option command instances to get fair price
	OptionCommand *price, *delta, *gamma;

	// Map with stock price as a key and the price/delta/gamma as value
	std::map<double, double> priceMap, deltaMap, gammaMap;

public:
	// Constructors and destructors
	FairValue() : data(OptionData()), Smin(0.0), Smax(0.0), dS(0.0)
	{	// Assigning price, delta, gamma pointers
		this->price = new CallPrice(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
		this->delta = new CallDelta(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
		this->gamma = new CallGamma(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
	}
	FairValue(const FairValue& fv) : data(fv.data), Smin(fv.Smin), Smax(fv.Smax), dS(fv.dS)
	{	// Assigning price, delta, gamma pointers	
		if (fv.data.type == 'C')
		{
			if (fv.data.style == 0)
			{
				this->price = new CallPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new CallDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new CallGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
			else if (fv.data.style == 1)
			{
				this->price = new ArithmeticAsianCallPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new ArithmeticAsianCallDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new ArithmeticAsianCallGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
			else if (fv.data.style == 2)
			{
				this->price = new GeometricAsianCallPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new GeometricAsianCallDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new GeometricAsianCallGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
		}
		else
		{
			if (fv.data.style == 0)
			{
				this->price = new PutPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new PutDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new PutGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
			else if (fv.data.style == 1)
			{
				this->price = new ArithmeticAsianPutPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new ArithmeticAsianPutDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new ArithmeticAsianPutGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
			else if (fv.data.style == 2)
			{
				this->price = new GeometricAsianPutPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new GeometricAsianPutDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new GeometricAsianPutGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
		}
		generateData(Smin, Smax, dS);
	}
	FairValue(const OptionData &op, double Smin, double Smax, double dS) :
		data(op), Smin(Smin), Smax(Smax), dS(dS)
	{	// Assigning price, delta, gamma pointers
		if (this->data.type == 'C')
		{
			if (this->data.style == 0)
			{
				this->price = new CallPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new CallDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new CallGamma(op.K, op.T, op.r, op.D, op.sigma);
			}
			else if (this->data.style == 1)
			{
				this->price = new ArithmeticAsianCallPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new ArithmeticAsianCallDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new ArithmeticAsianCallGamma(op.K, op.T, op.r, op.D, op.sigma);
			}
			else if (this->data.style == 2)
			{
				this->price = new GeometricAsianCallPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new GeometricAsianCallDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new GeometricAsianCallGamma(op.K, op.T, op.r, op.D, op.sigma);
			}
		}
		else
		{
			if (this->data.style == 0)
			{
				this->price = new PutPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new PutDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new PutGamma(op.K, op.T, op.r, op.D, op.sigma);
			}
			else if (this->data.style == 1)
			{
				this->price = new ArithmeticAsianPutPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new ArithmeticAsianPutDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new ArithmeticAsianPutGamma(op.K, op.T, op.r, op.D, op.sigma);
			}
			else if (this->data.style == 2)
			{
				this->price = new GeometricAsianPutPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new GeometricAsianPutDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new GeometricAsianPutGamma(op.K, op.T, op.r, op.D, op.sigma);
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

#endif // !FAIR_VALUE_HPP