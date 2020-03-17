
#ifndef BLACK_SCHOLES_HPP
#define BLACK_SCHOLES_HPP

#include "OptionCommand.hpp"
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <windows.h>

class BlackScholes
{
private:
	double K, r, T, D, sigma;
	double Smin, Smax, dS;
	char type; 
	bool vanilla;
	OptionCommand *price;
	OptionCommand *delta;
	OptionCommand *gamma;
	OptionCommand *theta;
	OptionCommand *vega;
	OptionCommand *rho;

	// TODO: comment
	std::map<double, double> prices;	// Stock price, standard deviation
	std::map<double, double> deltas;	// Stock price, standard error
	std::map<double, double> gammas;	// Stock price, option price
	std::map<double, double> thetas;	// Stock price, option delta
	std::map<double, double> vegas;		// Stock price, option gamma
	std::map<double, double> rhos;		// Stock price, option gamma


public:
	// CONSTRUCTORS AND DESTRUCTORS
	BlackScholes() : vanilla(true), K(0.0), r(0.0), T(0.0), D(0.0), sigma(0.0), type('C'), Smin(0.0), Smax(0.0), dS(0.0){};
	BlackScholes(const BlackScholes& bs);
	BlackScholes(bool vanilla, double K, double r, double T, double D, double sigma, char type, double Smin, double Smax, double dS);
	~BlackScholes();

	// GET FUNCTIONS 
	double getPrice(double S);
	double getDelta(double S);
	double getGamma(double S);
	double getTheta(double S);
	double getVega(double S);
	double getRho(double S);

	std::map<double, double> getPriceMap();
	std::map<double, double> getDeltaMap();
	std::map<double, double> getGammaMap();
	std::map<double, double> getThetaMap();
	std::map<double, double> getVegaMap();
	std::map<double, double> getRhoMap();

	// GENERATE FUNCTIONS
	void generateData(double Smin, double Smax, double dS);
	std::map<double, double> generatePrices(double Smin, double Smax, double dS);
	std::map<double, double> generateDeltas(double Smin, double Smax, double dS);
	std::map<double, double> generateGammas(double Smin, double Smax, double dS);
	std::map<double, double> generateThetas(double Smin, double Smax, double dS);
	std::map<double, double> generateVegas(double Smin, double Smax, double dS);
	std::map<double, double> generateRhos(double Smin, double Smax, double dS);

	void storeData();
	
	// DATA PROCESSING
	void writeToFile(std::map<double, double> myMap, std::string filename);

	// PLOT FUNCTIONS
/*	void plotPrices();
	void plotDeltas();
	void plotGammas();
	void plotThetas();
	void plotVegas();	
	void plotRhos();
	void plotGreeks();*/
};

#endif // ! BLACK_SCHOLES_HPP