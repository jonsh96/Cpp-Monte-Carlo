// OptionCommand.hpp
//
// Traditional GOF Command Pattern applied to one-factor 
// Black and Scholes exact prices and their greeks.
//
// (C) Datasim Education BV 2017
//
// https://www.macroption.com/black-scholes-formula/

// TODO: COMMENT

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

	// Normal variates etc.
	double n(double x)
	{
		double A = 1.0 / std::sqrt(2.0 * 3.1415);
		return A * std::exp(-x * x * 0.5);
	}

	double N(double x)
	{
		// The approximation to the cumulative normal distribution
		return cndN(x);
	}
};

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

// ----------------------------------------------------------------
// https://en.wikipedia.org/wiki/asian_option
// ----------------------------------------------------------------

class AsianCallPrice final : public OptionCommand
{
public:
	explicit AsianCallPrice(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~AsianCallPrice() {};

	virtual double execute(double S) override
	{
		double sig_g = sig / sqrt(3.0);
		double B = 0.5 * (r - 0.5 * sig_g * sig_g);
		double d1 = (log(S / K) + (B + 0.5 * sig_g * sig_g) * T) / (sig_g * sqrt(T));
		double d2 = d1 - sig_g * sqrt(T);
		return S * std::exp((B - r) * T) * N(d1) - K * std::exp(-r * T) * N(d2);
	}
};

class AsianPutPrice final : public OptionCommand
{
public:
	explicit AsianPutPrice(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual ~AsianPutPrice() {};

	virtual double execute(double S) override
	{
		double sig_g = sig / sqrt(3.0);
		double B = 0.5 * (r - 0.5 * sig_g * sig_g);
		double d1 = (log(S / K) + (B + 0.5 * sig_g * sig_g) * T) / (sig_g * sqrt(T));
		double d2 = d1 - sig_g * sqrt(T);

		return K * std::exp(-r * T) * N(-d2) - S * std::exp((B - r) * T) * N(-d1);
	}
};
// ----------------------------------------------------------------

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

#endif !OPTION_COMMAND_HPP