#ifndef SPPM_NORMAL_H_
#define SPPM_NORMAL_H_

#include "sppm.h"

#include <fstream>

// ========================== //

class SPPM_Normal: public SPPM {
	public:
		SPPM_Normal(lemon::SmartGraph& graph,
			lemon::SmartGraph::NodeMap<long long>& node_id,
			lemon::SmartGraph::NodeMap<Util::AttrMap>& node_attribute);

		void SetAttribute(std::string name);
		void SetNormalGammaParameters(double alpha, double beta, double m, double v);
	private:

		// Parameters
		double m_NG_alpha;
		double m_NG_beta;
		double m_NG_m;
		double m_NG_v;
		std::string m_attr;

		// Current state
		lemon::SmartGraph::NodeMap<double> m_mu;
		lemon::SmartGraph::NodeMap<double> m_tau;

		// Output files
		std::ofstream m_mu_file;
		std::ofstream m_tau_file;

		void PrepareOutputTheta();
		void FinishOutputTheta();
		void GenerateInitialTheta();
		void HoldTheta();
		void SampleTheta();

		double ComputeLogPredictive(int n, double sum_y, double sum_sq);
		double ComputeLogRatioPredictive(lemon::SmartGraph::NodeMap<bool>& set_u,
			lemon::SmartGraph::NodeMap<bool>& set_v);
};

// ========================== //

#endif // SPPM_NORMAL_H_
