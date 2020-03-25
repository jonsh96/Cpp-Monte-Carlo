<<<<<<< HEAD
#include "MonteCarlo.hpp" 
#include <sstream>
#include <math.h>
#include <tuple>
#include <stdexcept>

// Set functions
void MonteCarlo::setInitialPrice(double S)
{
	this->S0 = S;
	this->myOption.setInitialPrice(S);
}
void MonteCarlo::setMinimumPrice(double Smin) { this->Smin = Smin; }
void MonteCarlo::setMaximumPrice(double Smax) { this->Smax = Smax; };
void MonteCarlo::setNumberOfSteps(long NT) { this->NT = NT; }
void MonteCarlo::setStepSize(double dS) { this->dS = dS; }
void MonteCarlo::setNumberOfSimulations(long M) { this->M = M; }
void MonteCarlo::setOptionData(const OptionData& op) {this->myOption = op;}

// Get functions
double MonteCarlo::getOptionPrice() { return this->option_price; }
double MonteCarlo::getInitialPrice() { return this->S0; }
double MonteCarlo::getStandardDeviation() { return this->SD; }
double MonteCarlo::getStandardError() { return this->SE; }
long MonteCarlo::getNumberOfTimeSteps() { return this->NT; }
double MonteCarlo::getTimeElapsed() { return this->time_elapsed; }
long MonteCarlo::getNumberOfSimulations() { return this->M; }
int MonteCarlo::getSDEtype() { return this->SDE_type; }
char MonteCarlo::getOptionType() { return this->myOption.getType(); }
FairValue MonteCarlo::getFairOption() { return this->fairOption; }
std::map<double, double> MonteCarlo::getStdDev() { return this->stddev; }
std::map<double, double> MonteCarlo::getStdErr() { return this->stderror; }
std::map<double, double> MonteCarlo::getPrices() { return this->prices; }
std::map<double, double> MonteCarlo::getDeltas() { return this->deltas; }
std::map<double, double> MonteCarlo::getGammas() { return this->gammas; }

// Main functions
void MonteCarlo::run()
{
	// Generate the paths using path recycling (same Wiener process matrix for each price) 
	RNG randGen(this->NT, this->M);
	double dt = this->myOption.T / static_cast<double>(this->NT);

	this->dW = randGen.generateWienerProcesses(dt);
	
	// Generate stock paths and prices
	generatePrices(this->Smin, this->Smax, this->dS);
	
	// Calculate the delta and gamma with numerical methods for differentiation
	FDM finmethod(this->prices);
	this->deltas = finmethod.FOCD();
	this->gammas = finmethod.SOCD();
//	printSummary();
}

void MonteCarlo::rerun()
{
	// Clear all the data from previous run
	this->dW.clear();
	this->paths_plus.clear();
	this->paths_minus.clear();
	this->stddev.clear();
	this->stderror.clear();
	this->prices.clear();
	this->deltas.clear();
	this->gammas.clear();
	this->run();
}

void MonteCarlo::generatePaths(double S)
{
	SDE sde(this->myOption, SDE_type, NT);
	double M = static_cast<double>(this->M);
	std::tuple<std::vector<double>, std::vector<double>> myTuple;

	// Create a matrix to store the paths in
	std::vector<std::vector<double>> temp_paths_plus;
	std::vector<std::vector<double>> temp_paths_minus;

	// Loop through the number of simulations 
	for (long i = 1; i <= this->M; ++i)
	{
		myTuple = sde.generatePaths(S, dW[i]);
		
		// Store paths
		temp_paths_plus.push_back(std::get<0>(myTuple));
		temp_paths_minus.push_back(std::get<1>(myTuple));
	}
	// Store paths
	this->paths_plus = temp_paths_plus;
	this->paths_minus = temp_paths_minus;
	
	// Calculate the price 
	calculatePrice();
}

void MonteCarlo::generatePrices(double Smin, double Smax, double dS)
{
	// Generates prices, standard error and standard deviation and adds to maps
	// Initialise stopwatch
	StopWatch<> sw;
	sw.Start();

	// Loop through range of prices using dS as the jump
	for (double s = Smin; s < Smax; s += dS)
	{
		// Generate paths and add prices + standard deviation + standard error to a map
		generatePaths(s);
		prices.insert(std::pair<double, double>(s, this->option_price));
		stddev.insert(std::pair<double, double>(s, this->SD));
		stderror.insert(std::pair<double, double>(s, this->SE));
	}

	// Return time elapsed
	sw.Stop();
	this->time_elapsed = sw.GetTime();
}

// Calculation functions
void MonteCarlo::calculatePrice()
{
	// Initialise and define variables 
	double payoffT;
	double sumPriceT = 0.0; 
	double squaredPayoffT = 0.0;
	
	// Convert number of simulations to double for division later on
	double MC = static_cast<double>(this->M);

	// Loop through number of simulations, calculate payoff in OptionData
	for (long i = 1; i < this->M; i++)
	{
		// Send the entire path into myOption, there the price will be calculated whether
		// the option is pathwise dependent (e.g. Asian) or not (e.g. European)
		payoffT = 0.5 * (myOption.payoff(this->paths_plus[i]) + myOption.payoff(this->paths_minus[i]));
		sumPriceT += payoffT;
		squaredPayoffT += (payoffT * payoffT);
	}
	// Calculate standard deviation, standard error and option price
	this->option_price = std::exp(-myOption.r * myOption.T) * sumPriceT / MC;
	this->SD = std::sqrt((squaredPayoffT / MC) - (sumPriceT * sumPriceT) / (MC * MC));
	this->SE = this->SD / std::sqrt(M);
}
double MonteCarlo::maxPricingError()
{
	// Calculate the maximum pricing error in the range of prices
	double maxError = 0.0;
	double tempError;

	// Getting the Black Scholes prices from OptionData
	std::map<double, double> bsPrices = this->fairOption.getPriceMap();

	// Looping through the range of prices
	for (double s = this->Smin; s < this->Smax; s += this->dS)
	{	
		// Calculating the error at each stock price and comparing it to the current maximum
		tempError = std::abs(prices.at(s) - bsPrices.at(s));
		if (tempError > maxError)
			maxError = tempError;
	}
	return maxError;
}
double MonteCarlo::maxStandardError()
{
	// Calculating the maximum standard error
	double maxError = 0.0;
	double tempError;

	// Looping through the range of prices
	for (double s = this->Smin; s < this->Smax; s += this->dS)
	{	
		// Comparing the current standard error to the maximum
		tempError = this->stderror.at(s);
		if (tempError > maxError)
			maxError = tempError;
	}
	return maxError;
}
double MonteCarlo::maxStandardDeviation()
{	
	// Calculating the maximum standard deviation
	double maxError = 0.0;
	double tempError;

	// Looping through the range of prices
	for (double s = this->Smin; s < this->Smax; s += this->dS)
	{
		// Comparing the current standard deviation to the maximum
		tempError = this->stddev.at(s);
		if (tempError > maxError)
			maxError = tempError;
	}
	return maxError;
}
long MonteCarlo::minSimulationsNeeded()
{
	// Returns the minimum number of simulations needed to achieve an accuracy 
	// of "accuracy" within (1-alpha/2)% confidence level
	long N = std::lround(std::pow((this->maxStandardDeviation() * NormalCDFInverse(1.0 - this->alpha / 2.0) / this->accuracy), 2));
	return N;
}

// Print functions
/*void MonteCarlo::printSummary()
{
	std::cout << "----------------------------------";
	std::cout << "\nSummary of Monte Carlo simulation:";
	std::cout << "\n----------------------------------";
	if (this->SDE_type == 0)
		std::cout << "\nEuler method";
	else
		std::cout << "\Explicit method";
	std::cout << "\nMax pricing error:\t" << this->maxPricingError();
	std::cout << "\nMax standard error:\t" << this->maxStandardError();
	std::cout << "\nMax standard deviation:\t" << this->maxStandardDeviation();
	std::cout << "\nMin simulations needed:\t" << this->minSimulationsNeeded();
	std::cout << "\nTime elapsed:\t\t" << this->getTimeElapsed();
	std::cout << "\n----------------------------------\n";
}*/
std::ostream& operator<<(std::ostream& os, const MonteCarlo& MC)
{	
	// Overloaded print function
	std::string option_style, option_type;

	switch (MC.style)
	{
		case 0:
		{
			option_style = "European ";
			break;
		}
		case 1:
		{
			option_style = "Arithmetic Asian ";
			break;
		}
		case 2:
		{
			option_style = "Geometric Asian ";
			break;
		}
	}
	if (MC.myOption.type == 'C' || MC.myOption.type == 'c')
		option_type = "call option ";
	else
		option_type = "put option ";
	
	os << option_style << option_type << "simulations with parameters: \n(Smin, Smax, T, r, D, sigma, NT, M) = (";
	os << MC.Smin << ", " << MC.Smax << ", " << MC.myOption << ", " << MC.NT << ", " << MC.M << ")";
	return os;
}

// -----------------------------------------------
// Helper functions for normal cdf inverse from
// https://www.johndcook.com/blog/cpp_phi_inverse/
// -----------------------------------------------

double RationalApproximation(double t)
{
	// Abramowitz and Stegun formula 26.2.23.
	// The absolute value of the error should be less than 4.5 e-4.
	double c[] = { 2.515517, 0.802853, 0.010328 };
	double d[] = { 1.432788, 0.189269, 0.001308 };
	return t - ((c[2] * t + c[1]) * t + c[0]) / (((d[2] * t + d[1]) * t + d[0]) * t + 1.0);
}

double NormalCDFInverse(double p)
{
	if (p <= 0.0 || p >= 1.0)
	{
		std::stringstream os;
		os << "Invalid input argument (" << p
			<< "); must be larger than 0 but less than 1.";
		throw std::invalid_argument(os.str());
	}

	// See article above for explanation of this section.
	if (p < 0.5)
	{
		// F^-1(p) = - G^-1(p)
		return -RationalApproximation(sqrt(-2.0 * log(p)));
	}
	else
	{
		// F^-1(p) = G^-1(1-p)
		return RationalApproximation(sqrt(-2.0 * log(1 - p)));
	}
}
=======
#include "MonteCarlo.hpp" 
#include <sstream>
#include <math.h>
#include <tuple>
#include <stdexcept>

// Set functions
void MonteCarlo::setInitialPrice(double S)
{
	this->S0 = S;
	this->myOption.setInitialPrice(S);
}
void MonteCarlo::setMinimumPrice(double Smin) { this->Smin = Smin; }
void MonteCarlo::setMaximumPrice(double Smax) { this->Smax = Smax; };
void MonteCarlo::setNumberOfSteps(long NT) { this->NT = NT; }
void MonteCarlo::setStepSize(double dS) { this->dS = dS; }
void MonteCarlo::setNumberOfSimulations(long M) { this->M = M; }
void MonteCarlo::setOptionData(const OptionData& op) {this->myOption = op;}

// Get functions
double MonteCarlo::getOptionPrice() { return this->option_price; }
double MonteCarlo::getInitialPrice() { return this->S0; }
double MonteCarlo::getStandardDeviation() { return this->SD; }
double MonteCarlo::getStandardError() { return this->SE; }
long MonteCarlo::getNumberOfTimeSteps() { return this->NT; }
double MonteCarlo::getTimeElapsed() { return this->time_elapsed; }
long MonteCarlo::getNumberOfSimulations() { return this->M; }
int MonteCarlo::getSDEtype() { return this->SDE_type; }
char MonteCarlo::getOptionType() { return this->myOption.getType(); }
FairValue MonteCarlo::getFairOption() { return this->fairOption; }
std::map<double, double> MonteCarlo::getStdDev() { return this->stddev; }
std::map<double, double> MonteCarlo::getStdErr() { return this->stderror; }
std::map<double, double> MonteCarlo::getPrices() { return this->prices; }
std::map<double, double> MonteCarlo::getDeltas() { return this->deltas; }
std::map<double, double> MonteCarlo::getGammas() { return this->gammas; }

// Main functions
void MonteCarlo::run()
{
	// Generate the paths using path recycling (same Wiener process matrix for each price) 
	RNG randGen(this->NT, this->M);
	double dt = this->myOption.T / static_cast<double>(this->NT);

	this->dW = randGen.generateWienerProcesses(dt);
	
	// Generate stock paths and prices
	generatePrices(this->Smin, this->Smax, this->dS);
	
	// Calculate the delta and gamma with numerical methods for differentiation
	FDM finmethod(this->prices);
	this->deltas = finmethod.FOCD();
	this->gammas = finmethod.SOCD();
//	printSummary();
}

void MonteCarlo::rerun()
{
	// Clear all the data from previous run
	this->dW.clear();
	this->paths_plus.clear();
	this->paths_minus.clear();
	this->stddev.clear();
	this->stderror.clear();
	this->prices.clear();
	this->deltas.clear();
	this->gammas.clear();
	this->run();
}

void MonteCarlo::generatePaths(double S)
{
	SDE sde(this->myOption, SDE_type, NT);
	double M = static_cast<double>(this->M);
	std::tuple<std::vector<double>, std::vector<double>> myTuple;

	// Create a matrix to store the paths in
	std::vector<std::vector<double>> temp_paths_plus;
	std::vector<std::vector<double>> temp_paths_minus;

	// Loop through the number of simulations 
	for (long i = 1; i <= this->M; ++i)
	{
		myTuple = sde.generatePaths(S, dW[i]);
		
		// Store paths
		temp_paths_plus.push_back(std::get<0>(myTuple));
		temp_paths_minus.push_back(std::get<1>(myTuple));
	}
	// Store paths
	this->paths_plus = temp_paths_plus;
	this->paths_minus = temp_paths_minus;
	
	// Calculate the price 
	calculatePrice();
}

void MonteCarlo::generatePrices(double Smin, double Smax, double dS)
{
	// Generates prices, standard error and standard deviation and adds to maps
	// Initialise stopwatch
	StopWatch<> sw;
	sw.Start();

	// Loop through range of prices using dS as the jump
	for (double s = Smin; s < Smax; s += dS)
	{
		// Generate paths and add prices + standard deviation + standard error to a map
		generatePaths(s);
		prices.insert(std::pair<double, double>(s, this->option_price));
		stddev.insert(std::pair<double, double>(s, this->SD));
		stderror.insert(std::pair<double, double>(s, this->SE));
	}

	// Return time elapsed
	sw.Stop();
	this->time_elapsed = sw.GetTime();
}

// Calculation functions
void MonteCarlo::calculatePrice()
{
	// Initialise and define variables 
	double payoffT;
	double sumPriceT = 0.0; 
	double squaredPayoffT = 0.0;
	
	// Convert number of simulations to double for division later on
	double MC = static_cast<double>(this->M);

	// Loop through number of simulations, calculate payoff in OptionData
	for (long i = 1; i < this->M; i++)
	{
		// Send the entire path into myOption, there the price will be calculated whether
		// the option is pathwise dependent (e.g. Asian) or not (e.g. European)
		payoffT = 0.5 * (myOption.payoff(this->paths_plus[i]) + myOption.payoff(this->paths_minus[i]));
		sumPriceT += payoffT;
		squaredPayoffT += (payoffT * payoffT);
	}
	// Calculate standard deviation, standard error and option price
	this->option_price = std::exp(-myOption.r * myOption.T) * sumPriceT / MC;
	this->SD = std::sqrt((squaredPayoffT / MC) - (sumPriceT * sumPriceT) / (MC * MC));
	this->SE = this->SD / std::sqrt(M);
}
double MonteCarlo::maxPricingError()
{
	// Calculate the maximum pricing error in the range of prices
	double maxError = 0.0;
	double tempError;

	// Getting the Black Scholes prices from OptionData
	std::map<double, double> bsPrices = this->fairOption.getPriceMap();

	// Looping through the range of prices
	for (double s = this->Smin; s < this->Smax; s += this->dS)
	{	
		// Calculating the error at each stock price and comparing it to the current maximum
		tempError = std::abs(prices.at(s) - bsPrices.at(s));
		if (tempError > maxError)
			maxError = tempError;
	}
	return maxError;
}
double MonteCarlo::maxStandardError()
{
	// Calculating the maximum standard error
	double maxError = 0.0;
	double tempError;

	// Looping through the range of prices
	for (double s = this->Smin; s < this->Smax; s += this->dS)
	{	
		// Comparing the current standard error to the maximum
		tempError = this->stderror.at(s);
		if (tempError > maxError)
			maxError = tempError;
	}
	return maxError;
}
double MonteCarlo::maxStandardDeviation()
{	
	// Calculating the maximum standard deviation
	double maxError = 0.0;
	double tempError;

	// Looping through the range of prices
	for (double s = this->Smin; s < this->Smax; s += this->dS)
	{
		// Comparing the current standard deviation to the maximum
		tempError = this->stddev.at(s);
		if (tempError > maxError)
			maxError = tempError;
	}
	return maxError;
}
long MonteCarlo::minSimulationsNeeded()
{
	// Returns the minimum number of simulations needed to achieve an accuracy 
	// of "accuracy" within (1-alpha/2)% confidence level
	long N = std::lround(std::pow((this->maxStandardDeviation() * NormalCDFInverse(1.0 - this->alpha / 2.0) / this->accuracy), 2));
	return N;
}

// Print functions
/*void MonteCarlo::printSummary()
{
	std::cout << "----------------------------------";
	std::cout << "\nSummary of Monte Carlo simulation:";
	std::cout << "\n----------------------------------";
	if (this->SDE_type == 0)
		std::cout << "\nEuler method";
	else
		std::cout << "\Explicit method";
	std::cout << "\nMax pricing error:\t" << this->maxPricingError();
	std::cout << "\nMax standard error:\t" << this->maxStandardError();
	std::cout << "\nMax standard deviation:\t" << this->maxStandardDeviation();
	std::cout << "\nMin simulations needed:\t" << this->minSimulationsNeeded();
	std::cout << "\nTime elapsed:\t\t" << this->getTimeElapsed();
	std::cout << "\n----------------------------------\n";
}*/
std::ostream& operator<<(std::ostream& os, const MonteCarlo& MC)
{	
	// Overloaded print function
	std::string option_style, option_type;

	switch (MC.style)
	{
		case 0:
		{
			option_style = "European ";
			break;
		}
		case 1:
		{
			option_style = "Arithmetic Asian ";
			break;
		}
		case 2:
		{
			option_style = "Geometric Asian ";
			break;
		}
	}
	if (MC.myOption.type == 'C' || MC.myOption.type == 'c')
		option_type = "call option ";
	else
		option_type = "put option ";
	
	os << option_style << option_type << "simulations with parameters: \n(Smin, Smax, T, r, D, sigma, NT, M) = (";
	os << MC.Smin << ", " << MC.Smax << ", " << MC.myOption << ", " << MC.NT << ", " << MC.M << ")";
	return os;
}

// -----------------------------------------------
// Helper functions for normal cdf inverse from
// https://www.johndcook.com/blog/cpp_phi_inverse/
// -----------------------------------------------

double RationalApproximation(double t)
{
	// Abramowitz and Stegun formula 26.2.23.
	// The absolute value of the error should be less than 4.5 e-4.
	double c[] = { 2.515517, 0.802853, 0.010328 };
	double d[] = { 1.432788, 0.189269, 0.001308 };
	return t - ((c[2] * t + c[1]) * t + c[0]) / (((d[2] * t + d[1]) * t + d[0]) * t + 1.0);
}

double NormalCDFInverse(double p)
{
	if (p <= 0.0 || p >= 1.0)
	{
		std::stringstream os;
		os << "Invalid input argument (" << p
			<< "); must be larger than 0 but less than 1.";
		throw std::invalid_argument(os.str());
	}

	// See article above for explanation of this section.
	if (p < 0.5)
	{
		// F^-1(p) = - G^-1(p)
		return -RationalApproximation(sqrt(-2.0 * log(p)));
	}
	else
	{
		// F^-1(p) = G^-1(1-p)
		return RationalApproximation(sqrt(-2.0 * log(1 - p)));
	}
}
>>>>>>> 0099ef2e6bdf80a625b6b7fd410190d5ccaa3612
