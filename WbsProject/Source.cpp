// TestMC.cpp
//
// C++ code to price an option, essential algorithms.
//
// We take Geometric Brownian Motion (GBM) model and the Euler method. We compute option price.
//
// 2012-2-26 Update using std::vector<double> as data storage structure.
// 2016-4-3 DD using C++11 syntax, new example.
// 2017-8-2 DD version 1 for book
//
// (C) Datasim Education BV 2008-2018
//

#include <random>
#include <memory>
#include <cmath>
#include <iostream>
#include "StopWatch.cpp"
#include "OptionData.hpp" // in local directory
#include <vector>
class SDE
{ // Defines drift + diffusion + data
private:
	std::shared_ptr<OptionData> data;	// The data for the option
public:
	SDE(const OptionData& optionData) : data(new OptionData(optionData)) {}

	double drift(double t, double S)
	{ // Drift term	
		return (data->r - data->D) * S; // r - D
	}

	double diffusion(double t, double S)
	{ // Diffusion term
		return data->sig * S;
	}
};


int main()
{
	std::cout << "1 factor MC with explicit Euler\n";

	// Init via arg positon
	//OptionData(double strike, double expiration, double interestRate,
	//	double volatility, double dividend, int PC)
	//OptionData myOption { 100.0, 1.0, 0.06, 0.2, 0.03, 1 }; // Uniform initialisation

	// Init via named Boost variables
	OptionData myOption((OptionParams::strike = 100.0, OptionParams::expiration = 0.5,
		OptionParams::volatility = 0.4, OptionParams::dividend = 0.0,
		OptionParams::optionType = -1, OptionParams::interestRate = 0.06));
	//  OptionData myOption{ 65.0, 0.25, 0.08, 0.3, 0.0, -1 }; // Uniform initialisation
	//	OptionData myOption{ 110.0, 1.0, 0.05, 0.2, 0.0, -1 }; // Uniform initialisation
	myOption.K = 65.0;
	myOption.T = 0.25;
	myOption.r = 0.08;
	myOption.sig = 0.3;
	myOption.D = 0.0;
	myOption.type = -1;	// Put -1, Call +1


	myOption.K = 90.0;
	myOption.T = 0.5;
	myOption.r = 0.05;
	myOption.sig = 0.4;
	myOption.D = 0.0;
	myOption.type = -1;	// Put -1, Call +1

	SDE sde(myOption);

	// Initial value of SDE
	double S_0 = 60;

	// Variables for underlying stock
	double x;
	double VOld = S_0;
	double VNew;

	long NT = 100;
	std::cout << "Number of time steps: ";
	std::cin >> NT;

	// V2 mediator stuff
	long NSIM = 50000;
	std::cout << "Number of simulations: ";
	std::cin >> NSIM;
	double M = static_cast<double>(NSIM);

	double dt = myOption.T / static_cast<double> (NT);
	double sqrdt = std::sqrt(dt);

	// Normal random number
	double dW;
	double price = 0.0;	// Option price
	double payoffT;
	double avgPayoffT = 0.0;
	double squaredPayoff = 0.0;
	double sumPriceT = 0.0;

	// #include <random>
	// Normal (0,1) rng
	std::default_random_engine dre;
	std::normal_distribution<double> nor(0.0, 1.0);

	// Create a random number
	dW = nor(dre);

	StopWatch<> sw;
	sw.Start();


	std::vector<std::vector<double>> paths;

	for (long i = 1; i <= M; ++i)
	{ // Calculate a path at each iteration
		std::vector<double> path(NT + 1);

		if ((i / 100'000) * 100'000 == i)
		{// Give status after each 10000th iteration

			std::cout << i << ", ";
		}

		VOld = S_0;
		x = 0.0;

		for (long index = 0; index <= NT; ++index)
		{
			path[index] = VOld;
			// Create a random number
			dW = nor(dre);

			// The FDM (in this case explicit Euler), equation (9.2) from the text
			VNew = VOld + (dt * sde.drift(x, VOld)) + (sqrdt * sde.diffusion(x, VOld) * dW);

			VOld = VNew;
			x += dt;

		}
		path[NT + 1] = VOld;
		paths.push_back(path);

		// Assemble quantities (postprocessing)
		payoffT = myOption.myPayOffFunction(VNew);
		sumPriceT += payoffT;
		avgPayoffT += payoffT / M;
		avgPayoffT *= avgPayoffT;

		squaredPayoff += (payoffT * payoffT);
	}

	// Finally, discounting the average price
	price = std::exp(-myOption.r * myOption.T) * sumPriceT / M;

	std::cout << "Price, after discounting: " << paths[0][0]*10000 << ", " << std::endl;

	double SD = std::sqrt((squaredPayoff / M) - sumPriceT * sumPriceT / (M * M));
	std::cout << "Standard Deviation: " << SD << ", " << std::endl;

	double SE = SD / std::sqrt(M);
	std::cout << "Standard Error: " << SE << ", " << std::endl;

	sw.Stop();
	std::cout << "Elapsed time in seconds: " << sw.GetTime() << '\n';

	return 0;
}