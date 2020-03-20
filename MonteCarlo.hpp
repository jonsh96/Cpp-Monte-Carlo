#pragma once 

#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include "OptionData.hpp"
#include "StopWatch.cpp"
#include "SDE.hpp"
#include "FairValue.hpp"
#include <vector>
#include <random>
#include <memory>
#include <cmath>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <Windows.h>
#include "omp.h"
#include <algorithm>

// Helper functions
double RationalApproximation(double t);
double NormalCDFInverse(double p);

class MonteCarlo
{
private:
	double S0, SD, SE;
	double Smin, Smax, dS;
	long NT, M;
	double option_price, timeElapsed;
	double dollarAccuracy, percentile;
	bool isExact; 

	OptionData myOption;
	FairValue fairOption;
	std::vector<std::vector<double>> dW;
	std::vector<std::vector<double>> paths_plus;
	std::vector<std::vector<double>> paths_minus;

	std::map<double, double> stddev;	// Stock price, standard deviation
	std::map<double, double> stderror;	// Stock price, standard error
	std::map<double, double> prices;	// Stock price, option price
	std::map<double, double> deltas;	// Stock price, option delta
	std::map<double, double> gammas;	// Stock price, option gamma

public:
	//TODO: MORE CONSTRUCTORS AND DESTRUCTORS

	// CONSTRUCTORS AND DESTRUCTORS
	MonteCarlo(const MonteCarlo& MC);
	MonteCarlo(const OptionData& OD, double Smin, double Smax, double dS,
		long NT, long M, double percentile, double dollarAccuracy, bool isExact);

	//TODO: GENERATE (MORE?) GREEKS
	void generateData();
	void generateWienerProcesses();
	void generatePaths(double S);
	void generatePrices(double Smin, double Smax, double dS);
	void generateDeltas();
	void generateGammas();
	void storeData();

	// SET FUNCTIONS
	void setInitialPrice(double S);
	void setMinimumPrice(double Smin);
	void setMaximumPrice(double Smax);
	void setStepSize(double dS);
	void setNumberOfSteps(long NT);
	void setNumberOfSimulations(long M);
	void setOptionData(const OptionData& op);
	void setPaths(const std::vector<std::vector<double>>& mat);

	// GET FUNCTIONS
	double getOptionPrice();
	double getInitialPrice();
	double getStandardError();
	double getStandardDeviation();
	long getNumberOfTimeSteps();
	long getNumberOfSimulations();

//	OptionData getOptionData();
	// TODO: FINISH ALL GET FUNCTIONS
	// std::vector<std::vector<double>> getPaths();

	// CALCULATIONS
	void calculatePrice();

	// STATISTICS
	double maxPricingError();
	double maxStandardError();
	double maxStandardDeviation();
	long minSimulationsNeeded();
	//long simulationsNeeded(double alpha);
	//double pricingError();

	// DATA VISUALISATION
	// TODO: MAKE MAIN MORE USABLE
	// TODO: MAKE SAVE2TEXT/PLOTTING PROCESS MORE EFFICIENT AND REMOVE IT WHEN ITS NOT NEEDED
	void saveTitle();
	void printPathsToText();
	void plotPaths();
	void plotPrices();
	void plotDeltas();
	void plotGammas();
//	void plotThetas();
//	void plotVegas();	
//	void plotRhos();
//	void plotGreeks();

	friend std::ostream& operator<< (std::ostream& os, const MonteCarlo& MC);
	void refresh();
	void printSummary();
};

#endif MONTE_CARLO_HPP