#include "sppm.h"

#include "easylogging++.h"
#include <lemon/connectivity.h>
#include <lemon/kruskal.h>

using namespace std;
using namespace lemon;

// ==================================================== //

SPPM::SPPM(SmartGraph& G, SmartGraph::NodeMap<long long>& node_id,
	SmartGraph::NodeMap<Util::AttrMap>& node_attribute)

	: m_graph(G), m_node_id(node_id), m_node_attr(node_attribute),
	  m_pi(G), m_tree(G), m_u_group(G), m_v_group(G), m_rho_alpha(2),
	  m_rho_beta(5)  {

	LOG(INFO) << "== Initializing SPPM";
	m_pi_file.exceptions( ofstream::failbit | ofstream::badbit );
	m_tree_file.exceptions( ofstream::failbit | ofstream::badbit );
	m_rho_file.exceptions( ofstream::failbit | ofstream::badbit );

	for (SmartGraph::NodeIt u(m_graph); u != INVALID; ++u) {
		m_u_group[u] = false;
		m_v_group[u] = false;
	}
}

// ========================== //

SPPM::~SPPM() {
}

// ========================== //

void SPPM::SetRhoParameters(double alpha, double beta) {
	LOG(INFO) << "== Setting parameters: rho ~ Beta(alpha="<< alpha << ", beta=" << beta << ")";
	m_rho_alpha = alpha;
	m_rho_beta = beta;
}

// ========================== //

void SPPM::Run(int num_iter, int burn_in, int step_size) {
	LOG(INFO) << "== Running SPPM sampler for " << num_iter << " iterations";
	LOG(INFO) << " -- Burn-in: " << burn_in << " | Step size: " << step_size;

	// Prepare the outputs
	PrepareOutput();

	// Generate and store initial state
	GenerateInitialState();
	HoldSample();

	// Run the sampler
	LOG(INFO) << "== Starting now.";
	for (int iter = 1; iter <= num_iter; ++iter) {
		VLOG_EVERY_N(num_iter/100, 2) << " -- Iteration " << iter << " of " << num_iter;
		GetNewSample();
		if (iter > burn_in && (iter % step_size) == 0) {
			HoldSample();
		}
	}
	cout << endl;
	LOG(INFO) << "== Finished running SPPM sampler";

	// Finish the outputs
	FinishOutput();
}

// ========================== //

void SPPM::PrepareOutput() {
	LOG(INFO) << "== Preparing outputs";
	PrepareOutputPartition();
	PrepareOutputTree();
	PrepareOutputRho();
	PrepareOutputTheta();
}

// ========================== //

void SPPM::FinishOutput() {
	LOG(INFO) << "== Finishing outputs";
	FinishOutputPartition();
	FinishOutputRho();
	FinishOutputTheta();
	FinishOutputTree();
}

// ========================== //

void SPPM::GenerateInitialState() {
	LOG(INFO) << "== Generating initial state";
	GenerateInitialPartition();
	GenerateInitialRho();
	GenerateInitialTheta();
	GenerateInitialTree();
}

// ========================== //

void SPPM::HoldSample() {
	VLOG(3) << "== Holding sample (NOT YET IMPLEMENTED)";
	HoldPartition();
	HoldRho();
	HoldTheta();
	HoldTree();
}

// ========================== //

void SPPM::GetNewSample() {
	VLOG(3) << "== Getting new sample";
	SamplePartition();
	SampleRho();
	SampleTheta();
	SampleTree();
}

// ========================== //

void SPPM::PrepareOutputPartition() {
	LOG(INFO) << " -- Preparing output file 'pi.csv'";
	if (m_pi_file.is_open()) m_pi_file.close();
	m_pi_file.open("pi.csv", ofstream::out);
	bool first = true;
	for (SmartGraph::NodeIt u(m_graph); u != INVALID; ++u) {
		if (first) {
			m_pi_file << m_node_id[u];
			first = false;
		}
		else {
			m_pi_file << "," << m_node_id[u];
		}
	}
	m_pi_file << endl;
}

// ========================== //

void SPPM::PrepareOutputRho() {
	LOG(INFO) << " -- Preparing output file 'rho.csv'";
	if (m_rho_file.is_open()) m_rho_file.close();
	m_rho_file.open("rho.csv", ofstream::out);
	m_rho_file << "rho" << endl;
}

// ========================== //

void SPPM::PrepareOutputTree() {
	LOG(INFO) << " -- Preparing output file 'tree.csv'";
	if (m_tree_file.is_open()) m_tree_file.close();
	m_tree_file.open("tree.csv", ofstream::out);
	m_tree_file << "U_1,V_1";
	int curr = 2;
	while (curr < countNodes(m_graph)) {
		m_tree_file << ",U_" << curr << ",V_" << curr;
		++curr;
	}
	m_tree_file << endl;

}

// ========================== //

void SPPM::FinishOutputPartition() {
	LOG(INFO) << "== Closing output file 'pi.csv'";
	if (m_pi_file.is_open()) m_pi_file.close();
}

// ========================== //

void SPPM::FinishOutputRho() {
	LOG(INFO) << "== Closing output file 'rho.csv'";
	if (m_rho_file.is_open()) m_rho_file.close();
}

// ========================== //

void SPPM::FinishOutputTree() {
	LOG(INFO) << "== Closing output file 'tree.csv'";
	if (m_tree_file.is_open()) m_tree_file.close();
}

// ========================== //

void SPPM::GenerateInitialPartition() {
	LOG(INFO) << " -- Generating: partition";
	int grp = 0;
	for (SmartGraph::NodeIt u(m_graph); u != INVALID; ++u) {
		++grp;
		m_pi[u] = grp;
	}
	m_num_groups = grp;
}

// ========================== //

void SPPM::GenerateInitialRho() {
	// Generating rho
	LOG(INFO) << " -- Generating: rho";
	this->m_rho = Util::rbeta(m_rho_alpha, m_rho_beta, this->m_rng);
}

// ========================== //

void SPPM::GenerateInitialTree() {
	LOG(INFO) << " -- Generating: tree";
	uniform_real_distribution<float> unif(0.0, 1.0);

	// Put random weights on the edges
	SmartGraph::EdgeMap<float> cost_map(m_graph);
	for(SmartGraph::EdgeIt e(m_graph); e != INVALID; ++e) {
		cost_map[e] = unif(m_rng);
	}

	// Get a MST by Kruskal's algorithm
	kruskal(m_graph, cost_map, m_tree);

}

// ========================== //

void SPPM::HoldPartition() {
	VLOG(3) << " -- Holding Partition";
	try {
		bool first = true;
		for (SmartGraph::NodeIt u(m_graph); u != INVALID; ++u) {
			if (first) {
				m_pi_file << m_pi[u];
				first = false;
			}
			else {
				m_pi_file << "," << m_pi[u];
			}
		}
		m_pi_file << endl;
	} catch (...) {
		throw std::ios_base::failure("Failed to write partition to file.");
	}
}

// ========================== //

void SPPM::HoldRho() {
	VLOG(3) << " -- Holding Rho";
	try {
		m_rho_file << m_rho << endl;
	} catch (...) {
		throw std::ios_base::failure("Failed to write partition to file.");
	}
}

// ========================== //

void SPPM::HoldTree() {
	VLOG(3) << " -- Holding Tree";
	int count = 0;
	try {
		bool first = true;
		for (SmartGraph::EdgeIt e(m_graph); e != INVALID; ++e) {
			if (!m_tree[e]) continue;

			SmartGraph::Node u = m_graph.u(e);
			SmartGraph::Node v = m_graph.v(e);

			if (!first) m_tree_file << ",";
			else first = false;
			m_tree_file << m_node_id[u] << "," << m_node_id[v];
			count++;
		}
		m_tree_file << endl;
	} catch (...) {
		throw std::ios_base::failure("Failed to write partition to file.");
	}
	VLOG(3) << "# Edges written: " << count;
}

// ========================== //

double SPPM::ComputeLogRatio(FilteredGraph& filtered_graph,
	SmartGraph::Edge& e) {

	// Some helpers
	int n = countNodes(filtered_graph);
	int c = m_num_groups;

	// We must keep the filtered graph the same. But we remove the edge from it
	// in order to find the two groups formed by its removal. So we keep the
	// status to restore it afterwards
	bool status = filtered_graph.status(e);
	filtered_graph.disable(e);

	// Retrieve the nodes from the edge
	SmartGraph::Node u = m_graph.u(e);
	SmartGraph::Node v = m_graph.v(e);

	// Get the groups we need to compute the ratio
	FindNodes(filtered_graph, u, m_u_group);
	FindNodes(filtered_graph, v, m_v_group);

	// Compute the predictive
	double log_ratio_pred = ComputeLogRatioPredictive(m_u_group, m_v_group);

	// Compute ratio
	double ratio = log_ratio_pred;
	ratio += log(n + m_rho_beta - c) - log(c + m_rho_alpha - 2);

	// Restore the filtered graph (we disabled the edge, so if it wasn't
	// initially disabled we must restore it)
	filtered_graph.status(e, status);

	return ratio;
}

// ========================== //

void SPPM::SamplePartition() {
	VLOG(3) << " -- Sampling Partition";

	// Create an edge filter: on top of the tree, add or remove
	// edges according to the partitions
	SmartGraph::EdgeMap<bool> partition_filter(m_graph);
	for (SmartGraph::EdgeIt e(m_graph); e != INVALID; ++e) {
		SmartGraph::Node u = m_graph.u(e);
		SmartGraph::Node v = m_graph.v(e);
		bool in_tree = m_tree[e];
		bool same_group = m_pi[u] == m_pi[v];

		partition_filter[e] = in_tree && same_group;
	}

	// Create an adaptor from the filter we defined
	auto tree_graph = filterEdges(m_graph, m_tree);
	auto filtered_graph = filterEdges(m_graph, partition_filter);

	// For each edge, remove it or leave it.
	uniform_real_distribution<double> coin_toss(0.0, 1.0);
	for(FilteredGraph::EdgeIt e(tree_graph); e != INVALID; ++e) {
		bool was_there = filtered_graph.status(e);
		double log_ratio = ComputeLogRatio(filtered_graph, e);
		double coin = coin_toss(m_rng);
		if (log_ratio >= log((1.0 - coin) / coin)) {
			// Keep Edge
			partition_filter[e] = true;
			if (!was_there) m_num_groups--;
		}
		else {
			// Remove Edge
			partition_filter[e] = false;
			if (was_there) m_num_groups++;
		}
	}

	// Update the partition map
	int new_c = UpdatePi(filtered_graph);
	cout << "_(" << new_c << ")_" << flush;
}

// ========================== //


void SPPM::FindNodes(FilteredGraph& fg, SmartGraph::Node& s, SmartGraph::NodeMap<bool>& nodes) {

	for (SmartGraph::NodeIt u(m_graph); u != INVALID; ++u) {
		nodes[u] = false;
	}

	Bfs<FilteredGraph> bfs(fg);
	bfs.init();
	bfs.addSource(s);
	while (!bfs.emptyQueue()) {
		FilteredGraph::Node u = bfs.processNextNode();
		nodes[u] = true;
	}

}

// ========================== //

int SPPM::UpdatePi(FilteredGraph& fg) {
	long long group_id = 0;
	Bfs<FilteredGraph> bfs(fg);
	bfs.init();
	for (FilteredGraph::NodeIt n(fg); n != INVALID; ++n) {
		if (!bfs.reached(n)) {
			++group_id;
			bfs.addSource(n);
			while (!bfs.emptyQueue()) {
				FilteredGraph::Node u = bfs.processNextNode();
				m_pi[u] = group_id;
			}
		}
	}

	m_num_groups = group_id;

	return m_num_groups;
}

// ========================== //

void SPPM::SampleRho() {
	VLOG(3) << " -- Sampling Rho";
	int n = countNodes(m_graph);
	int c = m_num_groups;

	double alpha = m_rho_alpha + (c - 1);
	double beta = m_rho_beta + (n - c);
	m_rho = Util::rbeta(alpha, beta, m_rng);

	VLOG(4) << "  + New rho: " << m_rho;
}

// ========================== //

void SPPM::SampleTree() {
	VLOG(3) << " -- Sampling Tree";
	uniform_real_distribution<float> low_unif(0.0, 1.0);
	uniform_real_distribution<float> high_unif(5.0, 10.0);

	// Add the weights
	SmartGraph::EdgeMap<float> cost_map(m_graph);
	for(SmartGraph::EdgeIt e(m_graph); e != INVALID; ++e) {
		int p_u = m_pi[m_graph.u(e)];
		int p_v = m_pi[m_graph.v(e)];
		if (p_u == p_v) {
			cost_map[e] = low_unif(m_rng);
		}
		else {
			cost_map[e] = high_unif(m_rng);
		}
	}

	// Compute the MST through Kruskal
	kruskal(m_graph, cost_map, m_tree);
}

// ========================== //
