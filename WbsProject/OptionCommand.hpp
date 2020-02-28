// OptionCommand.hpp
//
// Traditional GOF Command Pattern applied to one-factor 
// Black and Scholes exact prices and their greeks.
//
// (C) Datasim Education BV 2017
//

#ifndef OptionCommand_HPP
#define OptionCommand_HPP

#include <memory>
#include <cmath>
#include <iostream>

// Normal variates etc.
double n(double x)
{
	double A = 1.0 / std::sqrt(2.0 * 3.1415);
	return A * std::exp(-x * x * 0.5);
}

// C++11 supports the error function
auto cndN = [](double x) { return 0.5 * (1.0 - std::erf(-x / std::sqrt(2.0))); };

double N(double x)
{ 
	// The approximation to the cumulative normal distribution
	return cndN(x);
}

class OptionCommand
{
private:

protected:
	double K;	double T;	double r;	double b;	double sig;

public:
	OptionCommand() = default;

	// Copy constructor
	// explicit constexpr OptionCommand(const OptionCommand &OC) : K(OC.K), T(OC.T), r(OC.r), b(OC.b), sig(OC.sig) {}

	// Constructor
	explicit OptionCommand(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: K(strike), T(expiration), r(riskFree), b(costOfCarry), sig(volatility) {}
	// Want to forbid copy constructor and assignment operator
	OptionCommand(const OptionCommand& c) = default;
	OptionCommand& operator = (const OptionCommand& c) = default;

	// The abstract interface
	virtual void execute(double S) = 0;

	// Implement as a function object; simple example of Template Method Pattern
	virtual void operator () (double S)
	{
		// Call derived class' execute()
		execute(S);
	}
};

class CallPrice final : public OptionCommand
{
public:
	explicit CallPrice(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		std::cout << "Call Price: " << (S * std::exp((b - r) * T) * N(d1)) - (K * std::exp(-r * T) * N(d2)) << '\n';
	}
};

class PutPrice final : public OptionCommand
{
public:
	explicit PutPrice(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		std::cout << "Put Price: " << (K * std::exp(-r * T) * N(-d2)) - (S * std::exp((b - r) * T) * N(-d1)) << '\n';
	}
};

class CallDelta final : public OptionCommand
{
public:
	explicit CallDelta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S)  override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;

		std::cout << "Call delta: " << std::exp((b - r) * T) * N(d1) << '\n';
	}
};

class PutDelta final : public OptionCommand
{
public:
	explicit PutDelta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;

		std::cout << "Put delta: " << std::exp((b - r) * T) * (N(d1) - 1.0) << '\n';
	}
};

class CallGamma final : public OptionCommand
{
public:
	explicit CallGamma(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;

		std::cout << "Call gamma: " << n(d1) * std::exp((b - r) * T) / (S * tmp) << '\n';
	}
};


class PutGamma final : public OptionCommand
{
public:
	explicit PutGamma(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;

		std::cout << "Put gamma: " << n(d1) * std::exp((b - r) * T) / (S * tmp) << '\n';
	}
};

class CallVega final : public OptionCommand
{
public:
	explicit CallVega(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;

		std::cout << "Call vega: " << S * std::exp((b - r) * T) * n(d1) * sqrt(T) << '\n';
	}
};


class PutVega final : public OptionCommand
{
public:
	explicit PutVega(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;

		std::cout << "Put vega: " << S * std::exp((b - r) * T) * n(d1) * sqrt(T) << '\n';
	}
};

class CallRho final : public OptionCommand
{
public:
	explicit CallRho(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		std::cout << "Call rho: " << T * K * std::exp(-r * T) * N(d2) << '\n';
	}
};

class PutRho final : public OptionCommand
{
public:
	explicit PutRho(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		std::cout << "Put rho: " << -T * K * std::exp(-r * T) * N(-d2) << '\n';
	}
};

class CallTheta final : public OptionCommand
{
public:
	explicit CallTheta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		double t1 = (S * std::exp((b - r) * T) * n(d1) * sig * 0.5) / std::sqrt(T);
		double t2 = (b - r) * (S * std::exp((b - r) * T) * N(d1));
		double t3 = r * K * std::exp(-r * T) * N(d2);

		std::cout << "Call theta: " << -(t1 + t2 + t3) << '\n';
	}
};

class PutTheta final : public OptionCommand
{
public:
	explicit PutTheta(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;
		double d2 = d1 - tmp;

		double t1 = -(S * std::exp((b - r) * T) * n(d1) * sig * 0.5) / std::sqrt(T);
		double t2 = (b - r) * (S * std::exp((b - r) * T) * N(-d1));
		double t3 = r * K * std::exp(-r * T) * N(-d2);

		std::cout << "Put theta: " << t1 + t2 + t3 << '\n';
	}
};


class CallElasticity final : public OptionCommand
{
public:
	explicit CallElasticity(double strike, double expiration, double riskFree, double costOfCarry, double volatility)
		: OptionCommand(strike, expiration, riskFree, costOfCarry, volatility) {}

	virtual void execute(double S) override
	{
		compute(S, 0.25);
	}

	virtual void compute(double S, double percentageMovement)
	{
		double tmp = sig * std::sqrt(T);
		double d1 = (log(S / K) + (b + (sig * sig) * 0.5) * T) / tmp;

		double cd = std::exp((b - r) * T) * N(d1);

		std::cout << "Call Elasticiy: " << (cd * S) / percentageMovement << '\n';
	}
};

#endif