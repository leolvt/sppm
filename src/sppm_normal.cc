#include "sppm_normal.h"

#include <cmath>

#include "easylogging++.h"

using namespace std;
using namespace lemon;

// ==================================================== //

SPPM_Normal::SPPM_Normal(SmartGraph& graph,
	SmartGraph::NodeMap<long long>& node_id,
	SmartGraph::NodeMap<Util::AttrMap>& node_attribute)
		: SPPM(graph, node_id, node_attribute), m_mu(graph), m_tau(graph) {
	LOG(INFO) << "== Initializing SPPM Normal";
}

// ========================== //

void SPPM_Normal::SetAttribute(std::string name) {
	m_attr = name;
}

// ========================== //

void SPPM_Normal::PrepareOutputTheta() {
	LOG(INFO) << " -- Preparing output file 'mu.csv'";
	if (m_mu_file.is_open()) m_mu_file.close();
	m_mu_file.open("mu.csv", ofstream::out);
	bool first = true;
	for (SmartGraph::NodeIt u(m_graph); u != INVALID; ++u) {
		if (first) {
			m_mu_file << m_node_id[u];
			first = false;
		}
		else {
			m_mu_file << "," << m_node_id[u];
		}
	}
	m_mu_file << endl;


	LOG(INFO) << " -- Preparing output file 'tau.csv'";
	if (m_tau_file.is_open()) m_tau_file.close();
	m_tau_file.open("tau.csv", ofstream::out);
	first = true;
	for (SmartGraph::NodeIt u(m_graph); u != INVALID; ++u) {
		if (first) {
			m_tau_file << m_node_id[u];
			first = false;
		}
		else {
			m_tau_file << "," << m_node_id[u];
		}
	}
	m_tau_file << endl;
}

// ========================== //

void SPPM_Normal::FinishOutputTheta() {
	LOG(INFO) << " -- Closing output file 'mu.csv'";
	if (m_mu_file.is_open()) m_mu_file.close();

	LOG(INFO) << " -- Closing output file 'tau.csv'";
	if (m_tau_file.is_open()) m_tau_file.close();
}

// ========================== //

void SPPM_Normal::SetNormalGammaParameters(double alpha, double beta, double m, double v) {
	LOG(INFO) << "== Setting parameters: mu, tau ~ NG(m=" << m << ", v=" << v << ", a=" << alpha << ", b=" << beta << ")";
	m_NG_alpha = alpha;
	m_NG_beta = beta;
	m_NG_m = m;
	m_NG_v = v;
}

// ========================== //

void SPPM_Normal::GenerateInitialTheta() {
	LOG(INFO) << " -- Generating: Mu & Tau";
	for(SmartGraph::NodeIt node(m_graph); node != INVALID; ++node) {
		m_tau[node] = Util::rgamma(m_NG_alpha, m_NG_alpha, m_rng);
		m_mu[node] = Util::rgamma(m_NG_m, m_NG_v * m_tau[node], m_rng);
	}
}

// ========================== //

void SPPM_Normal::HoldTheta() {
	VLOG(3) << " -- Holding Mu";
	bool first = true;
	for (SmartGraph::NodeIt u(m_graph); u != INVALID; ++u) {
		if (!first) m_mu_file << ",";
		else first = false;
		m_mu_file << m_mu[u];
	}
	m_mu_file << endl;

	VLOG(3) << " -- Holding Tau";
	first = true;
	for (SmartGraph::NodeIt u(m_graph); u != INVALID; ++u) {
		if (!first) m_tau_file << ",";
		else first = false;
		m_tau_file << m_tau[u];
	}
	m_tau_file << endl;
}

// ========================== //

void SPPM_Normal::SampleTheta() {
	VLOG(3) << " -- Sampling Mu & Tau (NOT YET IMPLEMENTED)";
	unordered_map<int, int> nk;
	unordered_map<int, double> mus;
	unordered_map<int, double> taus;
	unordered_map<int, double> sum_y;
	unordered_map<int, double> sum_y_sq;

	// Compute SUM_Y and SUM_Y^2
	for(SmartGraph::NodeIt node(m_graph); node != INVALID; ++node) {
		int grp = m_pi[node];
		double yi = m_node_attr[node][m_attr];
		sum_y[grp] += yi;
		sum_y_sq[grp] += yi * yi;
		nk[grp] += 1;
	}

	// Sample the values
	for(SmartGraph::NodeIt node(m_graph); node != INVALID; ++node) {
		int grp = m_pi[node];
		if (mus.find(grp) == mus.end()) {
			double a = m_NG_alpha + 0.5 * nk[grp];
			double b = m_NG_beta + 0.5*(sum_y_sq[grp] - pow(sum_y[grp], 2)/nk[grp]);
			b += 0.5*(nk[grp]*m_NG_v)/(nk[grp]+m_NG_v)*pow(1/nk[grp]*sum_y[grp] - m_NG_m, 2);
			double v = m_NG_v + nk[grp];
			double m = (m_NG_v*m_NG_m + sum_y[grp]) / (m_NG_v + nk[grp]);
			taus[grp] = Util::rgamma(a, b, m_rng);
			mus[grp] = Util::rnormal(m, v*taus[grp], m_rng);
		}
		m_mu[node] = mus[grp];
		m_tau[node] = taus[grp];
	}
}

// ========================== //

double SPPM_Normal::ComputeLogPredictive(int n, double sum_y, double sum_sq) {
	double a = m_NG_alpha;
	double b = m_NG_beta;
	double v = m_NG_v;
	double m = m_NG_m;
	//cout << "=====================" << endl;
	//cout << "a: " << a << endl;
	//cout << "b: " << b << endl;
	//cout << "v: " << v << endl;
	//cout << "m: " << m << endl;
	//cout << "n: " << n << endl;

	double result = 0.0;

	double base = b + 0.5*sum_sq + 0.5*v*m*m - 0.5 * pow(sum_y + v*m, 2) / (n + v);

	result = lgamma(a + n / 2.0) - lgamma(a);
	result += -n/2.0 * log(2 * M_PI);
	result += 0.5 * (log(v) - log(n + v));
	result += a * log(b);
	result += -(a + n/2.0) * log(base);

	//cout << "Sum(Y) = " << sum_y << endl;
	//cout << "Sum(Y²) = " << sum_sq << endl;
	//cout << result << endl;

	return result;
}

// ========================== //

double SPPM_Normal::ComputeLogRatioPredictive(
	lemon::SmartGraph::NodeMap<bool>& set_u,
	lemon::SmartGraph::NodeMap<bool>& set_v) {

	VLOG(5) << " -- Computing Log Ratio Predictive";

	double result = 0.0;

	int n_u = 0;
	int n_v = 0;
	int n = 0;

	double sum_y_u = 0.0;
	double sum_y_v = 0.0;
	double sum_y = 0.0;
	double sum_sq_u = 0.0;
	double sum_sq_v = 0.0;
	double sum_sq = 0.0;

	for(SmartGraph::NodeIt node(m_graph); node != INVALID; ++node) {
		if (set_u[node] == false) continue;
		double y = m_node_attr[node][m_attr];
		sum_y_u += y;
		sum_sq_u += y * y;
		n_u++;
	}

	for(SmartGraph::NodeIt node(m_graph); node != INVALID; ++node) {
		if (set_v[node] == false) continue;
		double y = m_node_attr[node][m_attr];
		sum_y_v += y;
		sum_sq_v += y * y;
		n_v++;
	}
	n = n_u + n_v;
	sum_y = sum_y_u + sum_y_v;
	sum_sq = sum_sq_u + sum_sq_v;

	result += ComputeLogPredictive(n, sum_y, sum_sq);
	result -= ComputeLogPredictive(n_u, sum_y_u, sum_sq_u);
	result -= ComputeLogPredictive(n_v, sum_y_v, sum_sq_v);

	return result;
}

// ==================================================== //
