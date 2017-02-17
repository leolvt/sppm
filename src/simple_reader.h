#ifndef SPPM_SIMPLE_READER_H_
#define SPPM_SIMPLE_READER_H_

#include <string>

#include <lemon/smart_graph.h>

// ========================== //

class SimpleReader {
	public:
		SimpleReader() { };
		virtual ~SimpleReader() { };
		bool LoadData(std::string filename, std::string attribute,
			lemon::SmartGraph& graph, lemon::SmartGraph::NodeMap<int>& node_id,
			lemon::SmartGraph::NodeMap<double>& y);
};

// ========================== //

#endif // SPPM_SIMPLE_READER_H_
