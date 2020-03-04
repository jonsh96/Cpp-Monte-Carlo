#include "SDE.hpp"

double SDE::drift(double t, double S)
{ // Drift term	
	return (data->r - data->D) * S; // r - D
}

double SDE::diffusion(double t, double S)
{ // Diffusion term
	return data->sigma * S;
}