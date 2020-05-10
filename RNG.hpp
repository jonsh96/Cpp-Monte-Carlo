#ifndef RNG_HPP
#define RNG_HPP

// Built-in header files
#include <vector>

/* ABOUT
	- Random number generator
	- Used to create Wiener process values in the Monte Carlo simulations
*/
class RNG
{
private:
	long NT, M;

public: 
	RNG(long NT, long M) : NT(NT), M(M) {}
	~RNG() {}
	std::vector<std::vector<double>> generateWienerProcesses(double dt);

};

#endif // !RNG_HPP

