#include "MonteCarlo.hpp" 
#include <sstream>
#include <math.h>
#include <tuple>
#include <stdexcept>

// -----------------------------------------------------------------------------------------
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
// -----------------------------------------------------------------------------------------

MonteCarlo::MonteCarlo(const MonteCarlo& MC)
{
	// Set the number of threads to use
	omp_set_num_threads(8);	// Number of threads = 8

	// Copy values over
	this->myOption = MC.myOption;
	this->S0 = MC.myOption.S0;
	this->Smin = MC.Smin;
	this->Smax = MC.Smax;
	this->dS = MC.dS;
	this->NT = MC.NT;
	this->M = MC.M;
	this->dollarAccuracy = MC.dollarAccuracy;
	this->percentile = MC.percentile;
	this->isExact = MC.isExact;

	// Initialise measurements
	this->SD = MC.SD;
	this->SE = MC.SE;
	this->timeElapsed = MC.timeElapsed;
	this->option_price = MC.option_price;

	// Create instance of the Black Scholes data structure
	BlackScholes BS(MC.myOption.vanilla, MC.myOption.K, MC.myOption.r, MC.myOption.T, MC.myOption.D, 
		MC.myOption.sigma, MC.myOption.type, MC.Smin, MC.Smax, MC.dS);
	this->bsOption = BS;

	// Start the Monte Carlo Process
	generateData();
}


// Default constructor
MonteCarlo::MonteCarlo(const OptionData& OD, double Smin, double Smax, double dS, 
	long NT, long M, double percentile, double dollarAccuracy, bool isExact)
{
	// Set the number of threads to use
	omp_set_num_threads(8);	// Number of threads = 8

	// Copy values over
	this->myOption = OD;
	this->S0 = OD.S0;
	this->Smin = Smin;
	this->Smax = Smax;
	this->dS = dS;
	this->NT = NT;
	this->M = M;
	this->percentile = percentile;
	this->dollarAccuracy = dollarAccuracy;
	this->isExact = isExact;

	// Initialise measurements
	this->SD = 0.0;
	this->SE = 0.0;
	this->timeElapsed = 0.0;
	this->option_price = 0.0;

	// Create instance of the Black Scholes data structure
	BlackScholes BS(OD.vanilla, OD.K, OD.r, OD.T, OD.D, OD.sigma, OD.type, Smin, Smax, dS);
	this->bsOption = BS;

	// Start the Monte Carlo Process
	generateData();
}

void MonteCarlo::generateData()
{
	// Generate the paths using path recycling (same Wiener process matrix for each price)
	generateWienerProcesses();
	generatePrices(this->Smin, this->Smax, this->dS);
	
	// Calculate the delta and gamma with numerical methods for differentiation
	generateDeltas();
	generateGammas();
}
void MonteCarlo::generateWienerProcesses()
{
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
	double dx;
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

		if (this->isExact)
		{
			// Initialise tuples
			VOld = std::make_tuple(S, S);
			VNew = std::make_tuple(0.0, 0.0);

			// x = 0.0;
			// Loop through the number of time steps
			for (long index = 0; index < NT; ++index)
			{
				std::get<0>(VNew) = std::get<0>(VOld) * std::exp(v * dt + this->myOption.sigma * dW[i][index]);
				std::get<1>(VNew) = std::get<1>(VOld) * std::exp(v * dt - this->myOption.sigma * dW[i][index]);
				//std::cout << std::exp(VOld) << "\n";
				temp_path_plus.push_back(std::get<0>(VOld));
				temp_path_minus.push_back(std::get<1>(VOld));

				std::get<0>(VOld) = std::get<0>(VNew);
				std::get<1>(VOld) = std::get<1>(VNew);
			}
		}
		else
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
		}
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
	this->timeElapsed = sw.GetTime();
}

void MonteCarlo::generateDeltas()
{
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

void MonteCarlo::storeData()
{
	// Write the maps (prices, deltas, gammas) to text file to plot data in Python
	if (this->isExact)
	{
		this->bsOption.writeToFile(this->prices, "MC_exact_prices.txt");
		this->bsOption.writeToFile(this->deltas, "MC_exact_deltas.txt");
		this->bsOption.writeToFile(this->gammas, "MC_exact_gammas.txt");
	}
	else
	{
		this->bsOption.writeToFile(this->prices, "MC_prices.txt");
		this->bsOption.writeToFile(this->deltas, "MC_deltas.txt");
		this->bsOption.writeToFile(this->gammas, "MC_gammas.txt");
	}
}

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

void MonteCarlo::setOptionData(const OptionData& op)
{
	this->myOption.setInitialPrice(op.S0);
	this->myOption.setStrike(op.K);
	this->myOption.setMaturity(op.T);
	this->myOption.setInterestRate(op.r);
	this->myOption.setVolatility(op.sigma);
	this->myOption.setDividend(op.D);
	this->myOption.setType(op.type);
	this->myOption.setOptionType(op.vanilla);
}

//void MonteCarlo::setPaths(const std::vector<std::vector<double>>& mat) { this->paths = mat; }

// Get functions
double MonteCarlo::getOptionPrice() { return this->option_price; }
double MonteCarlo::getInitialPrice() { return this->S0; }
double MonteCarlo::getStandardDeviation() { return this->SD; }
double MonteCarlo::getStandardError() { return this->SE; }
long MonteCarlo::getNumberOfTimeSteps() { return this->NT; }
long MonteCarlo::getNumberOfSimulations() { return this->M; }

// TODO: FIX THIS
//OptionData MonteCarlo::getOptionData()
//{
//	return this->myOption;
//}

//std::vector<std::vector<double>> MonteCarlo::getPaths() { return this->paths; }

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
	std::map<double, double> bsPrices = this->bsOption.getPriceMap();

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
	// Calculating the minimum number of simulations to achieve a X% accurate result within $Y
	// TODO: ADD INPUTS FOR ACCURACY
	long N = std::lround(std::pow((this->maxStandardDeviation() * NormalCDFInverse(this->percentile) / std::sqrt(this->dollarAccuracy)), 2));
	return N;
}

void MonteCarlo::saveTitle()
{
	// Writing the title for the plots
	std::ofstream title_file;
	title_file.open("title.txt");
	title_file << *this; // Using overloaded << operator 
	title_file.close();
}

/*void MonteCarlo::printPathsToText()
{
	// Write paths to text file in order to plot in Python
	std::ofstream path_file;
	path_file.open("paths.txt");
	for (long i = 0; i < this->M; i++)
	{
		for (long j = 0; j < this->NT; j++)  
		{
			path_file << paths[i][j];
			if (j < NT - 1)
				path_file << ",";
		}
		path_file << std::endl;
	}
	path_file.close();
}*/

void MonteCarlo::refresh()
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
}

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
	std::cout << "\nTime elapsed:\t\t" << this->timeElapsed;
	std::cout << "\n----------------------------------\n";
}

void MonteCarlo::plotPaths()
{
	// Plot paths using the system command to execute python file
	std::string command = "plot_test.py";
	system(command.c_str());
}

void MonteCarlo::plotPrices()
{
	// Plot price using the system command to execute python file
	std::string command = "plot_price.py";
	system(command.c_str());
}

void MonteCarlo::plotDeltas()
{
	// Plot deltas using the system command to execute python file
	std::string command = "plot_delta.py";
	system(command.c_str());
}

void MonteCarlo::plotGammas()
{
	// Plot gammas using the system command to execute python file
	std::string command = "plot_gamma.py";
	system(command.c_str());
}

/*  CANCELLED
	Would require a new Monte Carlo class/function

void MonteCarlo::plotThetas()
{
	// Use system command to execute python file
	std::string command = "python.exe plot_theta.py";
	system(command.c_str());
}

void MonteCarlo::plotVegas()
{
	// Use system command to execute python file
	std::string command = "python.exe plot_vega.py";
	system(command.c_str());
}

void MonteCarlo::plotRhos()
{
	// Use system command to execute python file
	std::string command = "python.exe plot_rho.py";
	system(command.c_str());
}

void MonteCarlo::plotGreeks()
{
	this->plotPrices();
	this->plotDeltas();
	this->plotGammas();
	this->plotThetas();
	this->plotVegas();
	this->plotRhos();
} */

std::ostream& operator<<(std::ostream& os, const MonteCarlo& MC)
{	
	// Overloaded print function
	os << "Simulations with parameters: \n(Smin, Smax, T, r, D, sigma, NT, M) = (";
	os << MC.Smin << ", " << MC.Smax << ", " << MC.myOption << ", " << MC.NT << ", " << MC.M << ")";
	return os;
}
