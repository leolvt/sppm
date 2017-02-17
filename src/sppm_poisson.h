#ifndef SPPM_POISSON_H_
#define SPPM_POISSON_H_

#include "sppm.h"

// ========================== //

class SPPM_Poisson: public SPPM {
	public:
		SPPM_Poisson(lemon::SmartGraph& graph,
			lemon::SmartGraph::NodeMap<long long>& node_id,
			lemon::SmartGraph::NodeMap<Util::AttrMap>& node_attribute);

		void SetAttributes(std::string response, std::string expected);
		void SetGammaParameters(double alpha, double beta);
	private:

		// Parameters
		double m_gamma_alpha;
		double m_gamma_beta;
		std::string m_attr_y;
		std::string m_attr_ei;

		// Current state
		lemon::SmartGraph::NodeMap<double> m_phi;

		// Output files
		std::ofstream m_phi_file;

		void PrepareOutputTheta();
		void FinishOutputTheta();
		void GenerateInitialTheta();
		void HoldTheta();
		void SampleTheta();

		double ComputeLogPredictive(double sum_y, double sum_ei);
		double ComputeLogRatioPredictive(lemon::SmartGraph::NodeMap<bool>& set_u,
			lemon::SmartGraph::NodeMap<bool>& set_v);
};

// ========================== //

#endif // SPPM_POISSON_H_
