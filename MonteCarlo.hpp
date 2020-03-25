#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include "OptionData.hpp"
#include "StopWatch.cpp"
#include "SDE.hpp"
#include "RNG.hpp"
#include "FDM.hpp"
#include "FairValue.hpp"
#include <vector>
#include <random>
#include <memory>
#include <cmath>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <Windows.h>
#include <algorithm>

double RationalApproximation(double t);
double NormalCDFInverse(double p);

class MonteCarlo
{
private:
	double S0, SD, SE, Smin, Smax, dS, option_price, time_elapsed, accuracy, alpha;
	long NT, M;
	int SDE_type, style; // 0 for Euler, 1 for exact simulation 
	OptionData myOption;
	FairValue fairOption;
	std::vector<std::vector<double>> dW, paths_plus, paths_minus;
	std::map<double, double> stddev, stderror, prices, deltas, gammas;

public:
	// Constructor and destructors
	MonteCarlo(const MonteCarlo& MC) : S0(MC.S0), SD(MC.SE), SE(MC.SE), Smin(MC.Smin), Smax(MC.Smax),
		dS(MC.dS), option_price(MC.option_price), time_elapsed(MC.time_elapsed), accuracy(MC.accuracy),
		alpha(MC.alpha), NT(MC.NT), M(MC.M), SDE_type(MC.SDE_type), style(MC.style), myOption(MC.myOption), 
		fairOption(MC.fairOption), dW(MC.dW), paths_plus(MC.paths_plus), paths_minus(MC.paths_minus), 
		stddev(MC.stddev), stderror(MC.stderror), prices(MC.prices), deltas(MC.deltas), gammas(MC.gammas)
	{
		// Create instance of the Black Scholes data structure
		FairValue FV(MC.myOption, MC.Smin, MC.Smax, MC.dS);
		this->fairOption = FV; 
	}

	MonteCarlo(const OptionData& OD, double Smin, double Smax, double dS, long NT, long M, 
		double alpha, double accuracy, int SDE_type, int style) : myOption(OD), S0(0.0), SD(0.0), SE(0.0), 
		Smin(Smin), Smax(Smax), dS(dS), option_price(0.0), time_elapsed(0.0), NT(NT), M(M), alpha(alpha), 
		accuracy(accuracy), SDE_type(SDE_type), style(style) {}

	// Set functions
	void setInitialPrice(double S);
	void setMinimumPrice(double Smin);
	void setMaximumPrice(double Smax);
	void setStepSize(double dS);
	void setNumberOfSteps(long NT);
	void setNumberOfSimulations(long M);
	void setOptionData(const OptionData& op);
	
	// Get functions
	double getOptionPrice();
	double getInitialPrice();
	double getStandardError();
	double getStandardDeviation();
	double getTimeElapsed();
	long getNumberOfTimeSteps();
	long getNumberOfSimulations();
	int getSDEtype();
	char getOptionType();
	FairValue getFairOption();
	std::map<double, double> getStdDev();	// Stock price, standard deviation
	std::map<double, double> getStdErr();	// Stock price, standard error
	std::map<double, double> getPrices();	// Stock price, option price
	std::map<double, double> getDeltas();	// Stock price, option delta
	std::map<double, double> getGammas();	// Stock price, option gamma

	// Main functions
	void run();
	void rerun();

	// Generate functions
	void generatePaths(double S);
	void generatePrices(double Smin, double Smax, double dS);
	//void generateDeltas();
	//void generateGammas();

	// Calculation functions
	void calculatePrice();
	double maxPricingError();
	double maxStandardError();
	double maxStandardDeviation();
	long minSimulationsNeeded();

	// Print functions
	// void printSummary();
	friend std::ostream& operator<< (std::ostream& os, const MonteCarlo& MC);
};

#endif MONTE_CARLO_HPP