#include <chrono>
#include <iostream>

#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a ## b
#define UNIQUE_NAME(base) CONCAT(base, __LINE__)

#define tic_msg(t, msg) auto t(new Timer(#msg));
#define tic(t) auto t(new Timer());
#define toc(t) delete t;
#define time(block_to_measure) tic(UNIQUE_NAME(t)) block_to_measure toc(UNIQUE_NAME(t))

struct Timer
{
	std::chrono::steady_clock::time_point begin;
	std::string msg_;

	Timer();
	Timer(std::string const& msg);
	~Timer();
};

Timer::Timer() : begin(std::chrono::steady_clock::now())
{}

Timer::Timer(std::string const& msg) : msg_(msg), begin(std::chrono::steady_clock::now())
{}

Timer::~Timer()
{
	auto end = std::chrono::steady_clock::now();
	std::cout
		<< msg_ + (msg_.size() > 0 ? ": " : "")
		<< "elapsed time is "
		<< std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1e+6
		<< " seconds"
		<< '\n';
}