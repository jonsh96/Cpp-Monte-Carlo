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
/*{ TODO: DELETE THIS
	this->myOption.setInitialPrice(op.S0);
	this->myOption.setStrike(op.K);
	this->myOption.setMaturity(op.T);
	this->myOption.setInterestRate(op.r);
	this->myOption.setVolatility(op.sigma);
	this->myOption.setDividend(op.D);
	this->myOption.setType(op.type);
	this->myOption.setOptionType(op.style);
}*/
// Get functions
double MonteCarlo::getOptionPrice() { return this->option_price; }
double MonteCarlo::getInitialPrice() { return this->S0; }
double MonteCarlo::getStandardDeviation() { return this->SD; }
double MonteCarlo::getStandardError() { return this->SE; }
long MonteCarlo::getNumberOfTimeSteps() { return this->NT; }
double MonteCarlo::getTimeElapsed() { return this->time_elapsed; }
long MonteCarlo::getNumberOfSimulations() { return this->M; }
int MonteCarlo::getSDEtype() { return this->SDE_type; }
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
	generateWienerProcesses();
	generatePrices(this->Smin, this->Smax, this->dS);
	
	// Calculate the delta and gamma with numerical methods for differentiation
	generateDeltas();
	generateGammas();
	printSummary();
}

/* TODO: DELETE THIS
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
}*/

void MonteCarlo::generateWienerProcesses()
{
	// Generates Wiener processes and stores as a matrix

	// Normal (0,1) rng
	std::mt19937 rd;
	std::default_random_engine generator(rd()); // rd() provides a random seed
	std::normal_distribution<double> distribution(0, 1);
	
	// Initialise matrix of Wiener paths
	std::vector<std::vector<double>> temp_paths;

	double dt = this->myOption.T / static_cast<double>(this->NT);
	double sqrdt = std::sqrt(dt);

	for (long i = 0; i <= this->M; ++i)
	{
		// Initialise temporary path
		std::vector<double> temp_path;
		for (long j = 0; j <= NT; ++j)
		{
			// Add a random normally distributed Wiener process to the Wiener path
			temp_path.push_back(sqrdt * distribution(generator));
		}
		temp_paths.push_back(temp_path);
	}
	// Set the member matrix dW as the paths generated
	this->dW = temp_paths;
}
void MonteCarlo::generatePaths(double S)
{
	// Define and initialise variables 
	double dt = this->myOption.T / static_cast<double>(this->NT);
	double sqrdt = std::sqrt(dt);
	double x = 0;
	double avgPayoffT = 0.0;
	double squaredPayoff = 0.0;
	double sumPriceT = 0.0;

	// Create an instance of the SDE
	SDE sde = SDE(this->myOption);

	// Create a matrix to store the paths in
	std::vector<std::vector<double>> temp_paths_plus;
	std::vector<std::vector<double>> temp_paths_minus;

	// Store M as a double to avoid problem when dividing later
	double M = static_cast<double>(this->M);
	double v = this->myOption.r - this->myOption.D - 0.5 * this->myOption.sigma * this->myOption.sigma;

	// Loop through the number of simulations 
	for (long i = 1; i <= this->M; ++i)
	{ 
		// Calculate a path at each iteration
		std::vector<double> temp_path_plus;
		std::vector<double> temp_path_minus;

		// Tuples to store plus and minus paths for AVR // TODO: COMMENT
		std::tuple <double, double> VOld;
		std::tuple <double, double> VNew;

		if (this->SDE_type == 0)
		{
			// Initialise tuples
			VOld = std::make_tuple(S, S);
			VNew = std::make_tuple(0.0, 0.0);

			// Loop through the number of time steps
			for (long index = 0; index < NT; ++index)
			{
				std::get<0>(VNew) = std::get<0>(VOld) * std::exp(v * dt + this->myOption.sigma * dW[i][index]);
				std::get<1>(VNew) = std::get<1>(VOld) * std::exp(v * dt - this->myOption.sigma * dW[i][index]);

				temp_path_plus.push_back(std::get<0>(VOld));
				temp_path_minus.push_back(std::get<1>(VOld));

				std::get<0>(VOld) = std::get<0>(VNew);
				std::get<1>(VOld) = std::get<1>(VNew);
			}
		}
		else if(this->SDE_type == 1)
		{
			// Initialise tuples
			VOld = std::make_tuple(S, S);
			VNew = std::make_tuple(0.0, 0.0);

			// Loop through the number of time steps
			for (long index = 0; index <= NT; ++index)
			{
				// TODO: COMMENT
				// The FDM (in this case explicit Euler), equation (9.2) from the text
				std::get<0>(VNew) = std::get<0>(VOld) + (dt * sde.drift(x, std::get<0>(VOld))) + (sde.diffusion(x, std::get<0>(VOld)) * dW[i][index]);
				std::get<1>(VNew) = std::get<1>(VOld) + (dt * sde.drift(x, std::get<1>(VOld))) - (sde.diffusion(x, std::get<1>(VOld)) * dW[i][index]);
				
				temp_path_plus.push_back(std::get<0>(VOld));
				temp_path_minus.push_back(std::get<1>(VOld));

				std::get<0>(VOld) = std::get<0>(VNew);
				std::get<1>(VOld) = std::get<1>(VNew);

				x += dt;
			}
		} // TODO: ADD MORE SDE_TYPES

		// Store path in matrix
		temp_path_plus.push_back(std::get<0>(VOld));
		temp_path_minus.push_back(std::get<1>(VOld));
		temp_paths_plus.push_back(temp_path_plus);
		temp_paths_minus.push_back(temp_path_minus);
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

void MonteCarlo::generateDeltas()
{
	// Generates deltas using finite difference methods

	// Define and initialise variables 
	double prev = 0.0;
	double next = 0.0;
	double delta;

	// If there are no prices available, delta cannot be calculated, therefore generatePrices is called
	if (this->prices.size() == 0)
	{
		generatePrices(Smin, Smax, dS);
	}

	// Loop through price map (not including boundary elements)
	for(double s = Smin+dS; s < Smax-dS; s += dS)
	{
		prev = this->prices.at(s - dS);
		next = this->prices.at(s + dS);

		// Delta (dC/dS) calculated using the centred finite-difference
		delta = (next - prev) / (2.0 * (this->dS));
		this->deltas.insert(std::pair<double, double>(s, delta));
	}
}
void MonteCarlo::generateGammas()
{
	// Generates gammas using finite difference methods

	// Define and initialise variables 
	double prev = 0.0;
	double curr = 0.0;
	double next = 0.0;
	double gamma;

	// If there are no prices available, gamma cannot be calculated, therefore generatePrices is called
	if (this->prices.size() == 0)
	{
		generatePrices(Smin, Smax, dS);
	}

	// Loop through price map (not including boundary elements)
	for (double s = Smin + dS; s < Smax - dS; s += dS)
	{
		prev = this->prices.at(s - dS);
		curr = this->prices.at(s);
		next = this->prices.at(s + dS);

		// Gamma (dC^2/(dS)^2 calculated with the second-order central difference method
		gamma = (next - 2.0 * curr + prev) / (this->dS * this->dS);
		this->gammas.insert(std::pair<double, double>(s,gamma));
	}
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
	this->SD = std::sqrt((squaredPayoffT / MC) - sumPriceT * sumPriceT / (MC * MC));
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
void MonteCarlo::printSummary()
{
	// Print summary of the previous run
	std::cout << "----------------------------------";
	std::cout << "\nSummary of Monte Carlo simulation:";
	std::cout << "\n----------------------------------";
	std::cout << "\nMax pricing error:\t" << this->maxPricingError();
	std::cout << "\nMax standard error:\t" << this->maxStandardError();
	std::cout << "\nMax standard deviation:\t" << this->maxStandardDeviation();
	std::cout << "\nMin simulations needed:\t" << this->minSimulationsNeeded();
	std::cout << "\nTime elapsed:\t\t" << this->getTimeElapsed();
	std::cout << "\n----------------------------------\n";
}
std::ostream& operator<<(std::ostream& os, const MonteCarlo& MC)
{	
	// Overloaded print function
	os << "Simulations with parameters: \n(Smin, Smax, T, r, D, sigma, NT, M) = (";
	os << MC.Smin << ", " << MC.Smax << ", " << MC.myOption << ", " << MC.NT << ", " << MC.M << ")";
	return os;
}

// Helper functions for normal cdf inverse from
// https://www.johndcook.com/blog/cpp_phi_inverse/

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
