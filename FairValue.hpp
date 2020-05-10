#ifndef FAIR_VALUE_HPP
#define FAIR_VALUE_HPP

// Built-in header files
#include <vector>
#include <map>
#include <string>
#include <fstream> 
#include <windows.h>

// Custom header files
#include "OptionData.hpp"
#include "OptionCommand.hpp"

/*	ABOUT
	- Assigns each option a price, delta and gamma pointer from OptionCommand
	- Generates a map with prices, deltas and gammas for a range of stock prices
*/

class FairValue
{
private:
	// Option parameters
	OptionData data;		
	double Smin, Smax, dS;

	// Option command instances to get fair price
	OptionCommand *price, *delta, *gamma;

	// Map with stock price as a key and the price/delta/gamma as value
	std::map<double, double> priceMap, deltaMap, gammaMap;

public:
	// Constructors and destructors 
	//	- Very messy implementation
	FairValue() : data(OptionData()), Smin(0.0), Smax(0.0), dS(0.0)
	{	// Assigning price, delta, gamma pointers
		{	// Assigning price, delta, gamma pointers	
			if (this->data.type == 'C')
			{	// Call options
				if (this->data.style == 0)
				{	// European
					this->price = new CallPrice(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->delta = new CallDelta(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->gamma = new CallGamma(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
				}
				else if (this->data.style == 1)
				{	// Arithmetic Asian
					this->price = new ArithmeticAsianCallPrice(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->delta = new ArithmeticAsianCallDelta(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->gamma = new ArithmeticAsianCallGamma(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
				}
				else if (this->data.style == 2)
				{	// Geometric Asian
					this->price = new GeometricAsianCallPrice(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->delta = new GeometricAsianCallDelta(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->gamma = new GeometricAsianCallGamma(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
				}
			}
			else
			{	// Put options
				if (this->data.style == 0)
				{	// European 
					this->price = new PutPrice(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->delta = new PutDelta(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->gamma = new PutGamma(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
				}
				else if (this->data.style == 1)
				{	// Arithmetic Asian
					this->price = new ArithmeticAsianPutPrice(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->delta = new ArithmeticAsianPutDelta(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->gamma = new ArithmeticAsianPutGamma(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
				}
				else if (this->data.style == 2)
				{	// Geometric
					this->price = new GeometricAsianPutPrice(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->delta = new GeometricAsianPutDelta(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
					this->gamma = new GeometricAsianPutGamma(this->data.K, this->data.T, this->data.r, this->data.D, this->data.sigma);
				}
				// TODO: Add more styles of options, e.g. barriers
			}
			// Generates the map data
			generateData(Smin, Smax, dS);
		}
	}
	FairValue(const FairValue& fv) : data(fv.data), Smin(fv.Smin), Smax(fv.Smax), dS(fv.dS)
	{	// Assigning price, delta, gamma pointers	
		if (fv.data.type == 'C')
		{	// Call options
			if (fv.data.style == 0)
			{	// European
				this->price = new CallPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new CallDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new CallGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
			else if (fv.data.style == 1)
			{	// Arithmetic Asian
				this->price = new ArithmeticAsianCallPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new ArithmeticAsianCallDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new ArithmeticAsianCallGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
			else if (fv.data.style == 2)
			{	// Geometric Asian
				this->price = new GeometricAsianCallPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new GeometricAsianCallDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new GeometricAsianCallGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
		}
		else
		{	// Put options
			if (fv.data.style == 0)
			{	// European 
				this->price = new PutPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new PutDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new PutGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
			else if (fv.data.style == 1)
			{	// Arithmetic Asian
				this->price = new ArithmeticAsianPutPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new ArithmeticAsianPutDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new ArithmeticAsianPutGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
			else if (fv.data.style == 2)
			{	// Geometric
				this->price = new GeometricAsianPutPrice(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->delta = new GeometricAsianPutDelta(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
				this->gamma = new GeometricAsianPutGamma(fv.data.K, fv.data.T, fv.data.r, fv.data.D, fv.data.sigma);
			}
			// TODO: Add more styles of options, e.g. barriers
		}
		// Generates the map data
		generateData(Smin, Smax, dS);
	}
	FairValue(const OptionData &op, double Smin, double Smax, double dS) :
		data(op), Smin(Smin), Smax(Smax), dS(dS)
	{	// Assigning price, delta, gamma pointers
		if (this->data.type == 'C')
		{	// Call option
			if (this->data.style == 0)
			{	// European
				this->price = new CallPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new CallDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new CallGamma(op.K, op.T, op.r, op.D, op.sigma);
			}
			else if (this->data.style == 1)
			{	// Arithmetic Asian
				this->price = new ArithmeticAsianCallPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new ArithmeticAsianCallDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new ArithmeticAsianCallGamma(op.K, op.T, op.r, op.D, op.sigma);
			}
			else if (this->data.style == 2)
			{	// Geometric Asian
				this->price = new GeometricAsianCallPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new GeometricAsianCallDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new GeometricAsianCallGamma(op.K, op.T, op.r, op.D, op.sigma);
			}
			// TODO: Add more styles
		}
		else
		{	// Put option
			if (this->data.style == 0)
			{	// European
				this->price = new PutPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new PutDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new PutGamma(op.K, op.T, op.r, op.D, op.sigma);
			}
			else if (this->data.style == 1)
			{	// Arithmetic Asian
				this->price = new ArithmeticAsianPutPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new ArithmeticAsianPutDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new ArithmeticAsianPutGamma(op.K, op.T, op.r, op.D, op.sigma);
			}
			else if (this->data.style == 2)
			{	// Geometric Asian
				this->price = new GeometricAsianPutPrice(op.K, op.T, op.r, op.D, op.sigma);
				this->delta = new GeometricAsianPutDelta(op.K, op.T, op.r, op.D, op.sigma);
				this->gamma = new GeometricAsianPutGamma(op.K, op.T, op.r, op.D, op.sigma);
			}
			// TODO: Add more styles
		}

		// Generates the data
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