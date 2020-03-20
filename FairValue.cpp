#include "FairValue.hpp"

// TODO: COMMENT
FairValue::FairValue(const FairValue& bs)
{
	this->vanilla = vanilla;
	this->K = bs.K;
	this->r = bs.r;
	this->T = bs.T;
	this->D = bs.D;
	this->sigma = bs.sigma;
	this->type = bs.type;
	this->Smin = bs.Smin;
	this->Smax = bs.Smax;
	this->dS = bs.dS;
	this->price = bs.price;
	this->delta = bs.delta;
	this->gamma = bs.gamma;
	this->theta = bs.theta;
	this->vega = bs.vega;	
	this->rho = bs.rho;
	this->prices = bs.prices;
	this->deltas = bs.deltas;
	this->gammas = bs.gammas;
	this->thetas = bs.thetas;
	this->vegas = bs.vegas;
	this->rhos = bs.rhos;
}

FairValue::FairValue(bool vanilla, double K, double r, double T, double D, double sigma, char type, double Smin, double Smax, double dS)
{
	this->vanilla = vanilla;
	this->K = K;
	this->r = r;
	this->T = T; 
	this->D = D;
	this->sigma = sigma;
	this->Smin = Smin;
	this->Smax = Smax;
	this->dS = dS;
	this->type = type;
	if (type == 'C')
	{
		if (this->vanilla)
		{
			this->price = new CallPrice(K, T, r, D, sigma);
			this->delta = new CallDelta(K, T, r, D, sigma);
			this->gamma = new CallGamma(K, T, r, D, sigma);
			this->theta = new CallTheta(K, T, r, D, sigma);
			this->vega = new CallVega(K, T, r, D, sigma);
			this->rho = new CallRho(K, T, r, D, sigma);
		}
		else
		{
			this->price = new AsianCallPrice(K, T, r, D, sigma); //TODO: ASIAN GREEKS
			this->delta = new AsianCallDelta(K, T, r, D, sigma);
			this->gamma = new AsianCallGamma(K, T, r, D, sigma);
			this->theta = new CallTheta(K, T, r, D, sigma);
			this->vega = new CallVega(K, T, r, D, sigma);
			this->rho = new CallRho(K, T, r, D, sigma);
		}
	}
	else
	{
		if (this->vanilla)
		{
			this->price = new PutPrice(K, T, r, D, sigma);
			this->delta = new PutDelta(K, T, r, D, sigma);
			this->gamma = new PutGamma(K, T, r, D, sigma);
			this->theta = new PutTheta(K, T, r, D, sigma);
			this->vega = new PutVega(K, T, r, D, sigma);
			this->rho = new PutRho(K, T, r, D, sigma);
		}
		else
		{
			this->price = new AsianPutPrice(K, T, r, D, sigma); // TODO: ASIAN GREEKS
			this->delta = new AsianPutDelta(K, T, r, D, sigma);
			this->gamma = new AsianPutGamma(K, T, r, D, sigma);
			this->theta = new PutTheta(K, T, r, D, sigma);
			this->vega = new PutVega(K, T, r, D, sigma);
			this->rho = new PutRho(K, T, r, D, sigma);
		}
	}
	generateData(Smin,Smax,dS);
	storeData();
}
FairValue::~FairValue()
{
/*	delete this->price;
	delete this->delta;
	delete this->gamma;
	delete this->theta;
	delete this->vega;
	delete this->rho*/
}

double FairValue::getPrice(double S)
{
	return this->price->execute(S);
}
double FairValue::getDelta(double S)
{
	return this->delta->execute(S);
}
double FairValue::getGamma(double S)
{
	return this->gamma->execute(S);
}
double FairValue::getTheta(double S)
{
	return this->theta->execute(S);
}
double FairValue::getVega(double S)
{
	return this->vega->execute(S);
}
double FairValue::getRho(double S)
{
	return this->rho->execute(S);
}
std::map<double, double> FairValue::getPriceMap()
{
	return this->prices;
}
std::map<double, double> FairValue::getDeltaMap()
{
	return this->deltas;
}
std::map<double, double> FairValue::getGammaMap()
{
	return this->gammas;
}
std::map<double, double> FairValue::getThetaMap()
{
	return this->thetas;
}
std::map<double, double> FairValue::getVegaMap()
{
	return this->vegas;
}
std::map<double, double> FairValue::getRhoMap()
{
	return this->rhos;
}

void FairValue::generateData(double Smin, double Smax, double dS)
{
	this->prices = generatePrices(Smin, Smax, dS);
	this->deltas = generateDeltas(Smin, Smax, dS);
	this->gammas = generateGammas(Smin, Smax, dS);
	this->thetas = generateThetas(Smin, Smax, dS);
	this->vegas = generateVegas(Smin, Smax, dS);
	this->rhos = generateRhos(Smin, Smax, dS);
}

std::map<double, double> FairValue::generatePrices(double Smin, double Smax, double dS)
{
	std::map<double, double> myMap;
	for (double s = Smin; s < Smax; s += dS)
	{
		myMap.insert(std::pair<double, double>(s, getPrice(s)));
	}
	return myMap;
}

std::map<double, double> FairValue::generateDeltas(double Smin, double Smax, double dS)
{
	std::map<double, double> myMap;
	for (double s = Smin; s < Smax; s += dS)
	{
		myMap.insert(std::pair<double, double>(s, getDelta(s)));
	}
	return myMap;
}

std::map<double, double> FairValue::generateGammas(double Smin, double Smax, double dS)
{
	std::map<double, double> myMap;
	for (double s = Smin; s < Smax; s += dS)
	{
		myMap.insert(std::pair<double, double>(s, getGamma(s)));
	}
	return myMap;
}

std::map<double, double> FairValue::generateThetas(double Smin, double Smax, double dS)
{
	std::map<double, double> myMap;
	for (double s = Smin; s < Smax; s += dS)
	{
		myMap.insert(std::pair<double, double>(s, getTheta(s)));
	}
	return myMap;
}

std::map<double, double> FairValue::generateVegas(double Smin, double Smax, double dS)
{
	std::map<double, double> myMap;
	for (double s = Smin; s < Smax; s += dS)
	{
		myMap.insert(std::pair<double, double>(s, getVega(s)));
	}
	return myMap;
}

std::map<double, double> FairValue::generateRhos(double Smin, double Smax, double dS)
{
	std::map<double, double> myMap;
	for (double s = Smin; s < Smax; s += dS)
	{
		myMap.insert(std::pair<double, double>(s, getRho(s)));
	}
	return myMap;
}

void FairValue::storeData()
{
	this->writeToFile(this->prices, "option_price.txt");
	this->writeToFile(this->deltas, "option_delta.txt");
	this->writeToFile(this->gammas, "option_gamma.txt");
	this->writeToFile(this->thetas, "option_theta.txt");
	this->writeToFile(this->vegas, "option_vega.txt");
	this->writeToFile(this->rhos, "option_rho.txt");
}

void FairValue::writeToFile(std::map<double, double> myMap, std::string filename)
{
	std::ofstream myFile;
	myFile.open(filename);
	for (auto it = myMap.begin(); it != myMap.end(); it++)
	{
		myFile << it->first << ", " << it->second << "\n";
	}
	myFile.close();
}

