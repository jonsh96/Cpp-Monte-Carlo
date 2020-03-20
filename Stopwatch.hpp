// StopWatch.hpp
//
// Simple stopwatch to measure real time wall clock time.
//
// (C) Datasim Education BV 2017
//

#ifndef StopWatch_HPP
#define StopWatch_HPP

#include<chrono>

template <typename TickType = double, typename UnitType = std::ratio<1, 1>>
class StopWatch {
private:
	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;

	bool isStart; // true:if start records the starting time of the operation;otherwise, false;
	bool isEnd;   // true if end records the ending time of the operation; otherwise,false;

public:
	StopWatch() :isStart(false), isEnd(false) {}
	StopWatch(const StopWatch<TickType, UnitType>& src) = delete;
	StopWatch<TickType, UnitType>& operator = (const StopWatch<TickType, UnitType>& src) = delete;

	void Start();
	void Stop();
	void Reset();			// Reset the time to NOW

	// Duration between start and stop in seconds; reset
	TickType GetTime();
};

#endif