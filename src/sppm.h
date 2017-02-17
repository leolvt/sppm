#ifndef SPPM_H_
#define SPPM_H_

#include <random>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

#include <lemon/adaptors.h>
#include <lemon/smart_graph.h>

#include "util.h"

// ========================== //

class SPPM {
	public:
		SPPM(lemon::SmartGraph& graph,
			lemon::SmartGraph::NodeMap<long long>& node_id,
			lemon::SmartGraph::NodeMap<Util::AttrMap>& node_attribute);
		virtual ~SPPM();

		void SetRhoParameters(double alpha, double beta);

		void Run(int num_iter, int burn_in, int step_size);

	protected:
		typedef std::unordered_set<lemon::SmartGraph::Node> NodeSet;

		lemon::SmartGraph& m_graph;
		lemon::SmartGraph::NodeMap<long long>& m_node_id;
		lemon::SmartGraph::NodeMap<Util::AttrMap>& m_node_attr;

		std::default_random_engine m_rng;

		int m_num_groups;

		// Current state
		double m_rho;
		lemon::SmartGraph::NodeMap<long long> m_pi;
		lemon::SmartGraph::EdgeMap<bool> m_tree;

	private:
		typedef lemon::FilterEdges<const lemon::SmartGraph> FilteredGraph;

		// Helper stuff
		lemon::SmartGraph::NodeMap<bool> m_u_group;
		lemon::SmartGraph::NodeMap<bool> m_v_group;

		// Output files
		std::ofstream m_pi_file;
		std::ofstream m_tree_file;
		std::ofstream m_rho_file;

		// Parameters
		double m_rho_alpha;
		double m_rho_beta;

		void PrepareOutput();
		void FinishOutput();
		void GenerateInitialState();
		void HoldSample();
		void GetNewSample();

		void PrepareOutputPartition();
		void PrepareOutputRho();
		void PrepareOutputTree();
		void GenerateInitialPartition();
		void GenerateInitialRho();
		void GenerateInitialTree();
		void FinishOutputPartition();
		void FinishOutputRho();
		void FinishOutputTree();
		void SamplePartition();
		void SampleRho();
		void SampleTree();
		void HoldPartition();
		void HoldRho();
		void HoldTree();

		//int UpdatePi(const Graph::EdgeFilter& edges);
		//double ComputeLogRatio(Graph::EdgeFilter& edges, int u, int v);
		void FindNodes(FilteredGraph& fg, lemon::SmartGraph::Node& s,
			lemon::SmartGraph::NodeMap<bool>& nodes);
		int UpdatePi(FilteredGraph& graph);
		double ComputeLogRatio(FilteredGraph& filtered_graph,
			lemon::SmartGraph::Edge& e);

		virtual void PrepareOutputTheta() = 0;
		virtual void FinishOutputTheta() = 0;
		virtual void GenerateInitialTheta() = 0;
		virtual void HoldTheta() = 0;
		virtual void SampleTheta() = 0;
		virtual double ComputeLogRatioPredictive(
			lemon::SmartGraph::NodeMap<bool>& set_u,
			lemon::SmartGraph::NodeMap<bool>& set_v) = 0;
};

// ========================== //

#endif // SPPM_H_
