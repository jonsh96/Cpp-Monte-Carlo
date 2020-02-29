#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include "OptionCommand.hpp"
#include "OptionData.hpp"
#include <vector>
#include <random>
#include <memory>
#include <cmath>
#include <iostream>
#include "StopWatch.cpp"
#include "SDE.hpp"
#include <fstream>
#include <stdio.h>
#include <Windows.h>

class MonteCarlo
{
private:
	//double S0; //_min, S_max, dS;
	long NT, M;
	double timeElapsed, option_price;
	OptionData myOption;
	OptionCommand *price;
	OptionCommand *delta;
	OptionCommand *gamma;
	OptionCommand *theta;
	OptionCommand *vega;
	OptionCommand *rho;
	std::vector<std::vector<double>> paths;

public:
	MonteCarlo(const OptionData& OD, long NT, long M)
	{
		this->myOption = OD;
		//this->S0 = S0;
		//this->S_min = S_min;
		//this->S_max = S_max;
		//this->dS = dS;
		this->NT = NT;
		this->M = M;
		this->timeElapsed = 0.0;
		this->option_price = 0.0;
		if (OD.type == 1)
		{
			this->price = new CallPrice(OD.K, OD.T, OD.r, OD.D, OD.sig);
			this->delta = new CallDelta(OD.K, OD.T, OD.r, OD.D, OD.sig);
			this->gamma = new CallGamma(OD.K, OD.T, OD.r, OD.D, OD.sig);
			this->theta = new CallTheta(OD.K, OD.T, OD.r, OD.D, OD.sig);
			this->vega = new CallVega(OD.K, OD.T, OD.r, OD.D, OD.sig);
			this->rho = new CallRho(OD.K, OD.T, OD.r, OD.D, OD.sig);
		}
		else
		{
			this->price = new PutPrice(OD.K, OD.T, OD.r, OD.D, OD.sig);
			this->delta = new PutDelta(OD.K, OD.T, OD.r, OD.D, OD.sig);
			this->gamma = new PutGamma(OD.K, OD.T, OD.r, OD.D, OD.sig);
			this->theta = new PutTheta(OD.K, OD.T, OD.r, OD.D, OD.sig);
			this->vega = new PutVega(OD.K, OD.T, OD.r, OD.D, OD.sig);
			this->rho = new PutRho(OD.K, OD.T, OD.r, OD.D, OD.sig);
		}
	}

	void generatePaths(double S0)
	{
		// Initialise stopwatch
		StopWatch<> sw;
		sw.Start();

		// Normal (0,1) rng
		std::default_random_engine dre;
		std::normal_distribution<double> nor(0.0, 1.0);

		// Create a random number
		double dW;
		double dt = this->myOption.T / static_cast<double>(this->NT);
		double sqrdt = std::sqrt(dt);
		double VOld, VNew, x;
		double payoffT;
		double avgPayoffT = 0.0;
		double squaredPayoff = 0.0;
		double sumPriceT = 0.0;

		// Create an instance of the SDE
		SDE sde = SDE(this->myOption);

		for (long i = 1; i <= this->M; ++i)
		{ // Calculate a path at each iteration

			std::vector<double> path;

			if ((i / 100'000) * 100'000 == i)
			{// Give status after each 10000th iteration

				std::cout << i << ", ";
			}

			VOld = S0;
			x = 0.0;
			for (long index = 0; index <= NT; ++index)
			{
				// Create a random number
				dW = nor(dre);

				// The FDM (in this case explicit Euler), equation (9.2) from the text
				VNew = VOld + (dt * sde.drift(x, VOld)) + (sqrdt * sde.diffusion(x, VOld) * dW);

				path.push_back(VOld);

				VOld = VNew;
				x += dt;
			}
			path.push_back(VOld);
			paths.push_back(path);

			// Assemble quantities (postprocessing)
			payoffT = myOption.payoff(path);
			sumPriceT += payoffT;
			avgPayoffT += payoffT / static_cast<double>(this->M);
			avgPayoffT *= avgPayoffT;
			squaredPayoff += (payoffT * payoffT);
		}
		this->option_price = std::exp(-this->myOption.r * this->myOption.T) * sumPriceT / this->M;
		this->paths = paths;
		// Measure time
		sw.Stop();
		this->timeElapsed = sw.GetTime();
	}

	//TODO: GENERATE GREEKS


	/* Not necessary
	void printPaths()
	{
		for (long i = 0; i < this->M; i++)
		{
			for (long j = 0; j < this->NT; j++)
			{
				std::cout << this->paths[i][j] << ",";
			}
			std::cout << std::endl;
		}
	}*/

	void printPathsToText()
	{
		// Text file - needs fstream and stdio
		std::ofstream myFile;
		myFile.open("paths.txt");

		for (long i = 0; i < this->M; i++)
		{
			for (long j = 0; j < this->NT; j++)
			{
				myFile << this->paths[i][j];
				if (j < this->NT - 1)
					myFile << ",";
			}
			myFile << std::endl;
		}
	}
	// test
	void plotPaths()
	{
		// Use system command to execute python file
		std::string command = "python.exe plot_test.py";
		system(command.c_str());
	}
};

#endif MONTE_CARLO_HPP