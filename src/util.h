#ifndef SPPM_UTIL_H_
#define SPPM_UTIL_H_

#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Util {

// ==================================================== //

typedef std::unordered_map<std::string, double> AttrMap;
typedef std::vector<std::pair<int, int>> Tree;
typedef std::function<double(int, int)> EdgeCallback;
typedef std::unordered_map<int, AttrMap>::iterator iterator;
typedef std::unordered_map<int, std::unordered_map<int, bool>> EdgeFilter;
typedef std::unordered_set<int> NodeSet;

// ========================== //

template<class URNG>
double rgamma(double shape, double rate, URNG& g) {
	std::gamma_distribution<double> gamma(shape, 1.0/rate);
	return gamma(g);
}

// ========================== //

template<class URNG>
double rbeta(double a, double b, URNG& g) {
	std::gamma_distribution<double> gamma_a(a, 1);
	std::gamma_distribution<double> gamma_b(b, 1);
	double x = gamma_a(g);
	double y = gamma_b(g);
	return x / (x + y);
}

// ========================== //

template<class URNG>
double rnormal(double mean, double prec, URNG& g) {
	std::normal_distribution<double> normal(mean, sqrt(1.0 / prec));
	return normal(g);
}

// ==================================================== //

};

#endif // SPPM_UTIL_H_
