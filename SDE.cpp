#include "SDE.hpp"

double SDE::drift(double t, double S)
{ 
	// Drift term	
	return (data->r - data->D) * S; // r - D
}

double SDE::diffusion(double t, double S)
{ 
	// Diffusion term
	return data->sigma * S;
}


double SDE::exact_drift(double t, double S)
{ 
	// Drift term	
	return (data->r - data->D - 0.5 * (data->sigma) * (data->sigma));
}

double SDE::exact_diffusion(double t, double S)
{ 
	// Diffusion term
	return data->sigma;
}