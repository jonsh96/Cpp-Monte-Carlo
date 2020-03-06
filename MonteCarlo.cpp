#include "MonteCarlo.hpp" 

// TODO: COPY CONSTRUCTOR

// TODO: COMMENT
MonteCarlo::MonteCarlo(const OptionData& OD, double Smin, double Smax, double dS, long NT, long M)
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

	// Initialise measurements
	this->SD = 0.0;
	this->SE = 0.0;
	this->timeElapsed = 0.0;
	this->option_price = 0.0;

	// Create instance of the Black Scholes data structure
	BlackScholes BS(OD.K, OD.r, OD.T, OD.D, OD.sigma, OD.type, Smin, Smax, dS);
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
	// TODO: allow for more random engines
	// std::default_random_engine dre;
	// std::normal_distribution<double> nor(0.0, 1.0);
	std::random_device rd;
	std::default_random_engine generator(rd()); // rd() provides a random seed
	std::normal_distribution<double> distribution(0, 1);
	//uniform_real_distribution<float> distribution(0, 1);
	
	// Initialise matrix of Wiener paths
	std::vector<std::vector<double>> temp_paths;

	for (long i = 0; i <= this->M; ++i)
	{
		// Initialise temporary path
		std::vector<double> temp_path;
		for (long j = 0; j <= NT; ++j)
		{
			// Add a random normally distributed Wiener process to the Wiener path
			temp_path.push_back(distribution(generator));
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
	double VOld, VNew, x;
	double avgPayoffT = 0.0;
	double squaredPayoff = 0.0;
	double sumPriceT = 0.0;

	// Create an instance of the SDE
	SDE sde = SDE(this->myOption);

	// Create a matrix to store the paths in
	std::vector<std::vector<double>> temp_paths;

	// Store M as a double to avoid problem when dividing later
	double M = static_cast<double>(this->M);

	// Loop through the number of simulations 
	for (long i = 1; i <= this->M; ++i)
	{ 
		// Calculate a path at each iteration
		std::vector<double> temp_path;

		VOld = S;
		x = 0.0;
	
		// Loop through the number of time steps
		for (long index = 0; index <= NT; ++index)
		{
			// TODO: COMMENT
			// The FDM (in this case explicit Euler), equation (9.2) from the text
			VNew = VOld + (dt * sde.drift(x, VOld)) + (sqrdt * sde.diffusion(x, VOld) * dW[i][index]);
			temp_path.push_back(VOld);
			VOld = VNew;
			x += dt;
		}
		// Store path in matrix
		temp_path.push_back(VOld);
		temp_paths.push_back(temp_path);
	}
	// Store paths
	this->paths = temp_paths;

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
	double prev = 0.0;
	double curr = 0.0;

	// If there are no prices available, delta cannot be calculated, therefore generatePrices is called
	if (this->prices.size() == 0)
	{
		generatePrices(Smin, Smax, dS);
	}

	// Loop through price map 
	for(auto it = this->prices.begin(); it != this->prices.end(); it++)
	{
		// Special case for first value as delta cannot be calculated theres
		if (it == this->prices.begin())
		{
			prev = it->second;
		}
		else
		{
			// TODO: COMMENT + TRY DIFFERENT METHOD
			curr = it->second;
			this->deltas.insert(std::pair<double, double>(it->first, (curr - prev)/this->dS));
			prev = curr;
		}
	}
}

void MonteCarlo::generateGammas()
{
	double prev = 0.0;
	double curr = 0.0;

	// If there are no gammas
	if (this->deltas.size() == 0)
	{
		generateDeltas();
	}
	for (auto it = this->deltas.begin(); it != this->deltas.end(); it++)
	{
		if (it == this->deltas.begin())
		{
			prev = it->second;
		}
		else
		{
			curr = it->second;
			this->gammas.insert(std::pair<double, double>(it->first, (curr - prev) / this->dS));
			prev = curr;
		}
	}
}

void MonteCarlo::storeData()
{
	this->bsOption.writeToFile(this->prices, "MC_prices.txt");
	this->bsOption.writeToFile(this->deltas, "MC_deltas.txt");
	this->bsOption.writeToFile(this->gammas, "MC_gammas.txt");
}

void MonteCarlo::setInitialPrice(double S)
{
	this->S0 = S;
	this->myOption.setInitialPrice(S);
}

void MonteCarlo::setNumberOfTimeSteps(long NT)
{
	this->NT = NT;
}

void MonteCarlo::setNumberOfSimulations(long M)
{
	this->M = M;
}

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

void MonteCarlo::setPaths(const std::vector<std::vector<double>>& mat)
{
	this->paths = mat;
}

double MonteCarlo::getInitialPrice()
{
	return this->S0;
}

double MonteCarlo::getStandardDeviation()
{
	return this->SD;
}

double MonteCarlo::getStandardError()
{
	return this->SE;
}

long MonteCarlo::getNumberOfTimeSteps()
{
	return this->NT;
}

long MonteCarlo::getNumberOfSimulations()
{
	return this->M;
}

// TODO: FIX THIS
//OptionData MonteCarlo::getOptionData()
//{
//	return this->myOption;
//}

std::vector<std::vector<double>> MonteCarlo::getPaths()
{
	return this->paths;
}

void MonteCarlo::calculatePrice()
{
	double payoffT;
	double sumPriceT = 0.0;
	double squaredPayoffT = 0.0;
	double MC = static_cast<double>(this->M);
	for (long i = 1; i < this->M; i++)
	{
		payoffT = myOption.payoff(this->paths[i]);
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
	// CALCULATIONS: INFINITY NORM
	// TODO: max general for two maps?
	double maxError = 0.0;
	double tempError;
	std::map<double, double> bsPrices = this->bsOption.getPriceMap();

	for (double s = this->Smin; s < this->Smax; s += this->dS)
	{
		tempError = std::abs(prices.at(s) - bsPrices.at(s));
		if (tempError > maxError)
			maxError = tempError;
	}
	return maxError;
}

double MonteCarlo::maxStandardError()
{
	double maxError = 0.0;
	double tempError;

	for (double s = this->Smin; s < this->Smax; s += this->dS)
	{
		tempError = this->stderror.at(s);
		if (tempError > maxError)
			maxError = tempError;
	}
	return maxError;
}

double MonteCarlo::maxStandardDeviation()
{
	double maxError = 0.0;
	double tempError;

	for (double s = this->Smin; s < this->Smax; s += this->dS)
	{
		tempError = this->stddev.at(s);
		if (tempError > maxError)
			maxError = tempError;
	}
	return maxError;
}
long MonteCarlo::minSimulationsNeeded()
{
	return std::lround(std::pow((this->maxStandardDeviation() * 1.96 / std::sqrt(0.01)), 2));
}

void MonteCarlo::printPathsToText()
{
	// Text file - needs fstream and stdio
	std::ofstream title_file;
	title_file.open("title.txt");

	title_file << "Simulations with parameters: \n(S(t), T, r, D, sigma, NT, M) = (";
	title_file << myOption;
	title_file << this->NT << ", ";
	title_file << this->M << ")";
	title_file.close();

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
}

void MonteCarlo::refresh()
{
	this->dW.clear();
	this->paths.clear();
	this->stddev.clear();
	this->stderror.clear();
	this->prices.clear();
	this->deltas.clear();
	this->gammas.clear();
}

// TODO: FIX 
void MonteCarlo::printSummary()
{
	std::cout << "----------------------------------";
	std::cout << "\nSummary of Monte Carlo simulation:";
	std::cout << "\n----------------------------------";
	std::cout << "\nMax pricing error:\t" << this->maxPricingError();
	std::cout << "\nMax standard error:\t" << this->maxStandardError();
	std::cout << "\nMax standard deviation:\t" << this->maxStandardDeviation();
	std::cout << "\nMin simulations needed:\t" << this->minSimulationsNeeded();

	//std::cout << "\nPrice of option:\t" << this->option_price;
	//std::cout << "\nBlack Scholes price:\t" << this->bsOption.getPrice(this->S0);
	std::cout << "\nTime elapsed:\t\t" << this->timeElapsed;
	std::cout << "\n----------------------------------\n";
}

void MonteCarlo::plotPaths()
{
	// Use system command to execute python file
	std::string command = "python.exe plot_test.py";
	system(command.c_str());
}

void MonteCarlo::plotPrices()
{
	// Use system command to execute python file
	std::string command = "python.exe plot_price.py";
	system(command.c_str());
}


void MonteCarlo::plotDeltas()
{
	// Use system command to execute python file
	std::string command = "python.exe plot_delta.py";
	system(command.c_str());
}

void MonteCarlo::plotGammas()
{
	// Use system command to execute python file
	std::string command = "python.exe plot_gamma.py";
	system(command.c_str());
}

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
}
