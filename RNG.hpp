#include <vector>

#ifndef RNG_HPP
#define RNG_HPP

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

