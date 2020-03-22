// OptionCommand.hpp
//
// Traditional GOF Command Pattern applied to one-factor 
// Black and Scholes exact prices and their greeks.
//
// (C) Datasim Education BV 2017
//
// Edited by Jón Sveinbjörn Halldórsson 2019
// - Edits include fixing some formulas for the Black Scholes formulas
//	 https://www.macroption.com/black-scholes-formula/
// - Adding the implementation for Asian call and put options
//   - Using analytical formulas from https://en.wikipedia.org/wiki/asian_option
//     to compare the accuracy of the Monte Carlo prices to an Asian option with
//	   geometric averaging instead of arithmatic averaging
//	- TODO: Add closed form solutions to barrier options 
//	 - Formulas from https://people.maths.ox.ac.uk/howison/barriers.pdf


//----------------------------------------------------------------------------------------------
//	Description and purpose of class
//	-	Calculates fair price/delta/gamma for comparison purposes using an inheritance hierarchy
//----------------------------------------------------------------------------------------------

#define PI atan(1.0)*4		// More accurate pi

#ifndef OPTION_COMMAND_HPP
#define OPTION_COMMAND_HPP

#include <memory>
#include <cmath>
#include <iostream>
#include <cmath>
#include <algorithm>

// C++11 supports the error function
auto cndN = [](double x) { return 0.5 * (1.0 - std::erf(-x / std::sqrt(2.0))); };

class OptionCommand
{
private:

protected:
	double K;	double T;	double r;	double b;	double sig;

public:
	OptionCommand() = default;
	virtual ~OptionCommand() {};
	// Copy constructor
	// explicit constexpr OptionCommand(const OptionCommand &OC) : K(OC.K), T(OC.T), r(OC.r), b(OC.b), sig(OC.sig) {}

	// Constructor
	explicit OptionCommand(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: K(strike), T(expiration), r(riskFree), b(costOfCarry), sig(volatility) {}

	// Want to forbid copy constructor and assignment operator
	OptionCommand(const OptionCommand& c) = default;
	OptionCommand& operator = (const OptionCommand& c) = default;

	// The abstract interface
	virtual double execute(double S) = 0;

	// Implement as a function object; simple example of Template Method Pattern
	virtual double operator () (double S)
	{
		// Call derived class' execute()
		return execute(S);
	}

	/*	The two following functions caused some linker issues when left as non-member functions within the class
		so I made them into member functions. The issues occurred when trying to include an instance of OptionCommand
		in a different class (BlackScholes) but only if the OptionCommand class was split into .cpp and .hpp, it worked 
		fine when it was only .hpp - found no other way to solve that linker issue */

	// Normal variates etc.
	double n(double x)
	{
		double A = 1.0 / std::sqrt(2.0 * PI);
		return A * std::exp(-x * x * 0.5);
	}

	double N(double x)
	{
		// The approximation to the cumulative normal distribution
		return cndN(x);
	}
};

// ------------------------------------------------------------------------
// Closed form solutions for arithmetic Asian option prices, deltas, gammas
// ------------------------------------------------------------------------
// Formulas taken from Haug - The Complete Guide to Option Pricing 2007
// Results are not very accurate 

class ArithmeticAsianCallPrice final : public OptionCommand
{
public:
	explicit ArithmeticAsianCallPrice(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~ArithmeticAsianCallPrice() {};

	virtual double execute(double S) override
	{
		double M1, M2;
		if (b != 0.0)
		{
			M1 = (std::exp(b * T) - 1.0) / (b * T);
			M2 = (2.0 * std::exp((2 * b + sig * sig) * T)) / ((b + sig * sig) * (2.0 * b + sig * sig) * (T * T));
			M2 += (2.0 / (b * T)) * (1.0 / (2 * b + sig * sig) - std::exp(b * T) / (b + sig * sig));
		}
		else
		{
			M1 = 1.0;
			M2 = 2.0 * std::exp(sig * sig * T) - 2.0 * (1.0 + sig * sig * T);
			M2 /= (sig * sig * sig * sig * T * T);
//				(2.0 * std::exp(sig * sig * T) - 2.0 * (1.0 + sig * sig * T)) / (sig * sig * sig * sig * T * T);
		}
		double b_a = static_cast<double>(log(M1) / T);
		double sig_a = std::sqrt(log(M2) / T - 2.0 * b_a);
		double d1 = (log(S / K) + (b_a + 0.5 * sig_a * sig_a) * T) / static_cast<double>(sig_a * sqrt(T));
		double d2 = d1 - sig_a * sqrt(T);
		return S * std::exp((b_a-r)* T) * N(d1) - K * std::exp(-r * T) * N(d2);
	}
};

class ArithmeticAsianPutPrice final : public OptionCommand
{
public:
	explicit ArithmeticAsianPutPrice(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~ArithmeticAsianPutPrice() {};

	virtual double execute(double S) override
	{
		double M1, M2;
		if (b != 0)
		{
			M1 = (std::exp(b * T) - 1.0) / (b * T);
			M2 = (2.0 * std::exp((2 * b + sig * sig) * T)) / ((b + sig * sig) * (2.0 * b + sig * sig) * (T * T));
			M2 += (2.0 / (b * T)) * (1.0 / (2 * b + sig * sig) - std::exp(b * T) / (b + sig * sig));
		}
		else
		{
			M1 = 1.0;
			M2 = (2.0 * std::exp(sig * sig * T) - 2.0 * (1.0 + sig * sig * T)) / (sig * sig * sig * sig * T * T);
		}
		double b_a = log(M1) / T;
		double sig_a = std::sqrt(log(M2) / T - 2.0 * b_a);
		double d1 = (log(S / K) + (b_a + 0.5 * sig_a * sig_a) * T) / (sig_a * sqrt(T));
		double d2 = d1 - sig_a * sqrt(T);
		return K * std::exp(-r * T) * N(-d2) - S * std::exp((b_a - r)* T) * N(-d1);
	}
};

class ArithmeticAsianCallDelta final : public OptionCommand
{
public:
	explicit ArithmeticAsianCallDelta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~ArithmeticAsianCallDelta() {};

	virtual double execute(double S)  override
	{
		double M1, M2;
		if (b != 0)
		{
			M1 = (std::exp(b * T) - 1.0) / (b * T);
			M2 = (2.0 * std::exp((2 * b + sig * sig) * T)) / ((b + sig * sig) * (2.0 * b + sig * sig) * (T * T));
			M2 += (2.0 / (b * T)) * (1.0 / (2 * b + sig * sig) - std::exp(b * T) / (b + sig * sig));
		}
		else
		{
			M1 = 1.0;
			M2 = (2.0 * std::exp(sig * sig * T) - 2.0 * (1.0 + sig * sig * T)) / (sig * sig * sig * sig * T * T);
		}
		double b_a = log(M1) / T;
		double sig_a = std::sqrt(log(M2) / T - 2.0 * b_a);
		double d1 = (log(S / K) + (b_a + 0.5 * sig_a * sig_a) * T) / (sig_a * sqrt(T));
		double d2 = d1 - sig_a * sqrt(T);

		return std::exp((b_a - r) * T) * N(d1);
	}
};

class ArithmeticAsianPutDelta final : public OptionCommand
{
public:
	explicit ArithmeticAsianPutDelta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~ArithmeticAsianPutDelta() {};

	virtual double execute(double S) override
	{
		double M1, M2;
		if (b != 0.0)
		{
			M1 = (std::exp(b * T) - 1.0) / (b * T);
			M2 = (2.0 * std::exp((2 * b + sig * sig) * T)) / ((b + sig * sig) * (2.0 * b + sig * sig) * (T * T));
			M2 += (2.0 / (b * T)) * (1.0 / (2 * b + sig * sig) - std::exp(b * T) / (b + sig * sig));
		}
		else
		{
			M1 = 1.0;
			M2 = (2.0 * std::exp(sig * sig * T) - 2.0 * (1.0 + sig * sig * T)) / (sig * sig * sig * sig * T * T);
		}
		double b_a = log(M1) / T;
		double sig_a = std::sqrt(log(M2) / T - 2.0 * b_a);
		double d1 = (log(S / K) + (b_a + 0.5 * sig_a * sig_a) * T) / (sig_a * sqrt(T));
		double d2 = d1 - sig_a * sqrt(T);

		return std::exp(b_a * T) * (N(d1) - 1);
	}
};


class ArithmeticAsianCallGamma final : public OptionCommand
{
public:
	explicit ArithmeticAsianCallGamma(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~ArithmeticAsianCallGamma() {};

	virtual double execute(double S) override
	{
		double M1, M2;
		if (b != 0)
		{
			M1 = (std::exp(b * T) - 1.0) / (b * T);
			M2 = (2.0 * std::exp((2 * b + sig * sig) * T)) / ((b + sig * sig) * (2.0 * b + sig * sig) * (T * T));
			M2 += (2.0 / (b * T)) * (1.0 / (2 * b + sig * sig) - std::exp(b * T) / (b + sig * sig));
		}
		else
		{
			M1 = 1.0;
			M2 = (2.0 * std::exp(sig * sig * T) - 2.0 * (1.0 + sig * sig * T)) / (sig * sig * sig * sig * T * T);
		}
		double b_a = log(M1) / T;
		double sig_a = std::sqrt(log(M2) / T - 2.0 * b_a);
		double d1 = (log(S / K) + (b_a + 0.5 * sig_a * sig_a) * T) / (sig_a * sqrt(T));
		double d2 = d1 - sig_a * sqrt(T);
		return n(d1) * std::exp((b_a - r) * T) / static_cast<double>(S * sig_a * sqrt(T));
	}
};

class ArithmeticAsianPutGamma final : public OptionCommand
{
public:
	explicit ArithmeticAsianPutGamma(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~ArithmeticAsianPutGamma() {};

	virtual double execute(double S) override
	{
		double M1, M2;
		if (b != 0)
		{
			M1 = (std::exp(b * T) - 1.0) / (b * T);
			M2 = (2.0 * std::exp((2 * b + sig * sig) * T)) / ((b + sig * sig) * (2.0 * b + sig * sig) * (T * T));
			M2 += (2.0 / (b * T)) * (1.0 / (2 * b + sig * sig) - std::exp(b * T) / (b + sig * sig));
		}
		else
		{
			M1 = 1.0;
			M2 = (2.0 * std::exp(sig * sig * T) - 2.0 * (1.0 + sig * sig * T)) / (sig * sig * sig * sig * T * T);
		}
		double b_a = log(M1) / T;
		double sig_a = std::sqrt(log(M2) / T - 2.0 * b_a);
		double d1 = (log(S / K) + (b_a + 0.5 * sig_a * sig_a) * T) / (sig_a * sqrt(T));
		double d2 = d1 - sig_a * sqrt(T);

		return n(d1) * std::exp((b_a-r) * T) / (S * sig_a * sqrt(T));
	}
};
// ------------------------------------------------------------------------
// Closed form solutions for geometric Asian option prices, deltas, gammas
// ------------------------------------------------------------------------
class GeometricAsianCallPrice final : public OptionCommand
{
public:
	explicit GeometricAsianCallPrice (double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~GeometricAsianCallPrice() {};

	virtual double execute(double S) override
	{
		double sig_g = sig / sqrt(3.0);
		double B = 0.5 * (r - b - 0.5 * sig_g * sig_g);
		double d1 = (log(S / K) + (B + 0.5 * sig_g * sig_g) * T) / (sig_g * sqrt(T));
		double d2 = d1 - sig_g * sqrt(T);
		return S * std::exp((B - r) * T) * N(d1) - K * std::exp(-r * T) * N(d2);
	}
};

class GeometricAsianPutPrice final : public OptionCommand
{
public:
	explicit GeometricAsianPutPrice(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~GeometricAsianPutPrice() {};

	virtual double execute(double S) override
	{
		double sig_g = sig / sqrt(3.0);
		double B = 0.5 * (r - b - 0.5 * sig_g * sig_g);
		double d1 = (log(S / K) + (B + 0.5 * sig_g * sig_g) * T) / (sig_g * sqrt(T));
		double d2 = d1 - sig_g * sqrt(T);

		return K * std::exp(-r * T) * N(-d2) - S * std::exp((B - r) * T) * N(-d1);
	}
};

class GeometricAsianCallDelta final : public OptionCommand
{
public:
	explicit GeometricAsianCallDelta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~GeometricAsianCallDelta() {};

	virtual double execute(double S)  override
	{
		double sig_g = sig / sqrt(3.0);
		double B = 0.5 * (r - b - 0.5 * sig_g * sig_g);
		double d1 = (log(S / K) + (B + 0.5 * sig_g * sig_g) * T) / (sig_g * sqrt(T));

		return std::exp((B - r) * T) * N(d1);
	}
};

class GeometricAsianPutDelta final : public OptionCommand
{
public:
	explicit GeometricAsianPutDelta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~GeometricAsianPutDelta() {};

	virtual double execute(double S) override
	{
		double sig_g = sig / sqrt(3.0);
		double B = 0.5 * (r - b - 0.5 * sig_g * sig_g);
		double d1 = (log(S / K) + (B + 0.5 * sig_g * sig_g) * T) / (sig_g * sqrt(T));
		double d2 = d1 - sig_g * sqrt(T);

		return std::exp((B - r) * T) * (N(d1) - 1);
	}
};


class GeometricAsianCallGamma final : public OptionCommand
{
public:
	explicit GeometricAsianCallGamma(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~GeometricAsianCallGamma() {};

	virtual double execute(double S) override
	{
		double sig_g = sig / sqrt(3.0);
		double B = 0.5 * (r - b - 0.5 * sig_g * sig_g);
		double d1 = (log(S / K) + (B + 0.5 * sig_g * sig_g) * T) / (sig_g * sqrt(T));

		return n(d1) * std::exp(-b * T) / (S * sig_g * sqrt(T));
	}
};

class GeometricAsianPutGamma final : public OptionCommand
{
public:
	explicit GeometricAsianPutGamma(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~GeometricAsianPutGamma() {};

	virtual double execute(double S) override
	{
		double sig_g = sig / sqrt(3.0);
		double B = 0.5 * (r - b - 0.5 * sig_g * sig_g);
		double d1 = (log(S / K) + (B + 0.5 * sig_g * sig_g) * T) / (sig_g * sqrt(T));

		return n(d1) * std::exp(-b * T) / (S * sig_g * sqrt(T));
	}
};

// ----------------------------------------------------------------
// Closed form solutions for European option prices, deltas, gammas
// ----------------------------------------------------------------
class CallPrice final : public OptionCommand
{
public:
	explicit CallPrice(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~CallPrice() {};

	virtual double execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (r - b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		return std::exp(-r * T) * ((S * std::exp((r - b) * T) * N(d1)) - K * N(d2));
	}
};

class PutPrice final : public OptionCommand
{
public:
	explicit PutPrice(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~PutPrice() {};

	virtual double execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (r - b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		return std::exp(-r * T) * (K * N(-d2) - (S * std::exp((r - b) * T) * N(-d1)));
	}
};

class CallDelta final : public OptionCommand
{
public:
	explicit CallDelta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}
	
	virtual ~CallDelta() {};

	virtual double execute(double S)  override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (r - b + (sig * sig) * 0.5) * T) / tmp;

		return std::exp(-b * T) * N(d1);
	}
};

class PutDelta final : public OptionCommand
{
public:
	explicit PutDelta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~PutDelta() {};

	virtual double execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (r - b + (sig * sig) * 0.5) * T) / tmp;

		return std::exp(-b * T) * (N(d1) - 1.0);
	}
};

class CallGamma final : public OptionCommand
{
public:
	explicit CallGamma(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~CallGamma() {};

	virtual double execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (r - b + (sig * sig) * 0.5) * T) / tmp;

		return n(d1) * std::exp(-b * T) / (S * tmp);
	}
};

class PutGamma final : public OptionCommand
{
public:
	explicit PutGamma(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~PutGamma() {};

	virtual double execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (r - b + (sig * sig) * 0.5) * T) / tmp;

		return n(d1) * std::exp(-b * T) / (S * tmp);
	}
};

class CallVega final : public OptionCommand
{
public:
	explicit CallVega(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~CallVega() {};

	virtual double execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (r - b + (sig * sig) * 0.5) * T) / tmp;

		return S * std::exp((b - r) * T) * n(d1) * sqrt(T);
	}
};

class PutVega final : public OptionCommand
{
public:
	explicit PutVega(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~PutVega() {};
	
	virtual double execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (r - b + (sig * sig) * 0.5) * T) / tmp;

		return S * std::exp((b - r) * T) * n(d1) * sqrt(T);
	}
};

class CallRho final : public OptionCommand
{
public:
	explicit CallRho(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~CallRho() {};

	virtual double execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (r - b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		return T * K * std::exp(-r * T) * N(d2);
	}
};

class PutRho final : public OptionCommand
{
public:
	explicit PutRho(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~PutRho() {};

	virtual double execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (r - b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		return -T * K * std::exp(-r * T) * N(-d2);
	}
};

class CallTheta final : public OptionCommand
{
public:
	explicit CallTheta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~CallTheta() {};

	virtual double execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (r - b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		double t1 = (S * std::exp((b - r) * T) * n(d1) * sig * 0.5) / std::sqrt(T);
		double t2 = (b - r) * (S * std::exp((b - r) * T) * N(d1));
		double t3 = r * K * std::exp(-r * T) * N(d2);

		return -(t1 + t2 + t3);
	}
};

class PutTheta final : public OptionCommand
{
public:
	explicit PutTheta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~PutTheta() {};

	virtual double execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		double t1 = -(S * std::exp((b - r) * T) * n(d1) * sig * 0.5) / std::sqrt(T);
		double t2 = (b - r) * (S * std::exp((b - r) * T) * N(-d1));
		double t3 = r * K * std::exp(-r * T) * N(-d2);

		return t1 + t2 + t3;
	}
};

class CallElasticity final : public OptionCommand
{
public:
	explicit CallElasticity(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~CallElasticity() {};

	virtual double execute(double S) override
	{
		compute(S, 0.25);
	}

	virtual double compute(double S, double percentageMovement)
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;

		double cd = std::exp((b - r) * T) * N(d1);

		return (cd * S) / percentageMovement;
	}
};

// ---------------------------------------------------------------------
// TODO: Closed form solutions for Barrier option prices, deltas, gammas
// ---------------------------------------------------------------------

#endif !OPTION_COMMAND_HPP