#include "sppm_poisson.h"

#include <cmath>

#include "easylogging++.h"

using namespace std;
using namespace lemon;

// ==================================================== //

SPPM_Poisson::SPPM_Poisson(SmartGraph& graph,
	SmartGraph::NodeMap<long long>& node_id,
	SmartGraph::NodeMap<Util::AttrMap>& node_attribute)
		: SPPM(graph, node_id, node_attribute), m_phi(graph) {
	LOG(INFO) << "== Initializing SPPM Poisson";
}

// ========================== //

void SPPM_Poisson::SetAttributes(string response, string expected) {
	m_attr_y = response;
	m_attr_ei = expected;
}

// ========================== //

void SPPM_Poisson::PrepareOutputTheta() {
	LOG(INFO) << " -- Preparing output file 'phi.csv'";
	if (m_phi_file.is_open()) m_phi_file.close();
	m_phi_file.open("phi.csv", ofstream::out);
	bool first = true;
	for (SmartGraph::NodeIt u(m_graph); u != INVALID; ++u) {
		if (first) {
			m_phi_file << m_node_id[u];
			first = false;
		}
		else {
			m_phi_file << "," << m_node_id[u];
		}
	}
	m_phi_file << endl;
}

// ========================== //

void SPPM_Poisson::FinishOutputTheta() {
	LOG(INFO) << " -- Closing output file 'phi.csv'";
	if (m_phi_file.is_open()) m_phi_file.close();
}

// ========================== //

void SPPM_Poisson::SetGammaParameters(double alpha, double beta) {
	LOG(INFO) << "== Setting parameters: phi ~ Gamma(alpha=" << alpha << ", beta=" << beta << ")";
	m_gamma_alpha = alpha;
	m_gamma_beta = beta;
}

// ========================== //

void SPPM_Poisson::GenerateInitialTheta() {
	LOG(INFO) << " -- Generating: Phi";
	for(SmartGraph::NodeIt node(m_graph); node != INVALID; ++node) {
		double alpha = m_gamma_alpha;
		double beta = m_gamma_beta;
		m_phi[node] = Util::rgamma(alpha, beta, m_rng);
	}
}

// ========================== //

void SPPM_Poisson::HoldTheta() {
	VLOG(3) << " -- Holding Phi";
	bool first = true;
	for (SmartGraph::NodeIt u(m_graph); u != INVALID; ++u) {
		if (first) {
			m_phi_file << m_phi[u];
			first = false;
		}
		else {
			m_phi_file << "," << m_phi[u];
		}
	}
	m_phi_file << endl;
}

// ========================== //

void SPPM_Poisson::SampleTheta() {
	VLOG(3) << " -- Sampling Phi";
	unordered_map<int, double> phis;
	unordered_map<int, double> sum_y;
	unordered_map<int, double> sum_ei;

	// Compute SUM_Y and SUM_EI
	for(SmartGraph::NodeIt node(m_graph); node != INVALID; ++node) {
		int grp = m_pi[node];
		sum_y[grp] += m_node_attr[node][m_attr_y];
		sum_ei[grp] += m_node_attr[node][m_attr_ei];
	}

	// Sample the values
	for(SmartGraph::NodeIt node(m_graph); node != INVALID; ++node) {
		int grp = m_pi[node];
		if (phis.find(grp) == phis.end()) {
			double alpha = m_gamma_alpha + sum_y[grp];
			double beta = m_gamma_beta + sum_ei[grp];
			phis[grp] = Util::rgamma(alpha, beta, m_rng);
		}
		m_phi[node] = phis[grp];
	}
}

// ========================== //

double SPPM_Poisson::ComputeLogPredictive(double sum_y, double sum_ei) {
	double a = m_gamma_alpha;
	double b = m_gamma_beta;

	double result = 0.0;

	result += a*log(b) + lgamma(a + sum_y);
	result -= (a+sum_y)*log(b+sum_ei) + lgamma(a);

	LOG(DEBUG) << "-----------------";
	LOG(DEBUG) << " SY = " << sum_y << "   SE = " << sum_ei;
	LOG(DEBUG) << "  a = " << a <<"   b = " << b;
	LOG(DEBUG) << " -> Result: " << result;

	return result;
}

// ========================== //

double SPPM_Poisson::ComputeLogRatioPredictive(
	lemon::SmartGraph::NodeMap<bool>& set_u,
	lemon::SmartGraph::NodeMap<bool>& set_v) {

	VLOG(5) << " -- Computing Log Ratio Predictive";

	double result = 0.0;

	double sum_y_u = 0.0;
	double sum_y_v = 0.0;
	double sum_y = 0.0;
	double sum_ei_u = 0.0;
	double sum_ei_v = 0.0;
	double sum_ei = 0.0;

	for(SmartGraph::NodeIt node(m_graph); node != INVALID; ++node) {
		if (set_u[node] == false) continue;
		double y = m_node_attr[node][m_attr_y];
		double ei = m_node_attr[node][m_attr_ei];
		sum_y_u += y;
		sum_ei_u += ei;
	}

	for(SmartGraph::NodeIt node(m_graph); node != INVALID; ++node) {
		if (set_v[node] == false) continue;
		double y = m_node_attr[node][m_attr_y];
		double ei = m_node_attr[node][m_attr_ei];
		sum_y_v += y;
		sum_ei_v += ei;
	}
	sum_y = sum_y_u + sum_y_v;
	sum_ei = sum_ei_u + sum_ei_v;

	double full = ComputeLogPredictive(sum_y, sum_ei);
	double u = ComputeLogPredictive(sum_y_u, sum_ei_u);
	double v = ComputeLogPredictive(sum_y_v, sum_ei_v);

	result = full - u - v;
	LOG(DEBUG) << "======================";
	LOG(DEBUG) << sum_y_u << "  __  " << sum_y_v << " -- ";
	LOG(DEBUG) << sum_ei_u << "  __  " << sum_ei_v << " -- ";
	LOG(DEBUG) << u << "  __  " << v << " -- " << full;
	LOG(DEBUG) << result;

	return result;
}

// ==================================================== //
