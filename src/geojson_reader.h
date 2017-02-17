#ifndef SPPM_GEOJSON_READER_H_
#define SPPM_GEOJSON_READER_H_

#include <string>

#include <lemon/smart_graph.h>

#include "util.h"

// ========================== //

class GeoJSONReader {
	public:
		GeoJSONReader() { };
		virtual ~GeoJSONReader() { };
		//bool LoadData(std::string filename, Graph& graph);

		bool LoadData(std::string filename, lemon::SmartGraph& graph,
			lemon::SmartGraph::NodeMap<long long>& node_id,
			lemon::SmartGraph::NodeMap<Util::AttrMap>& node_attribute);

		//bool LoadData(std::string filename, std::string attribute,
			//lemon::SmartGraph& graph, lemon::SmartGraph::NodeMap<int>& node_id,
			//lemon::SmartGraph::NodeMap<double>& y);

		//bool LoadData(std::string filename, std::string attribute,
			//lemon::SmartGraph& graph, lemon::SmartGraph::NodeMap<int>& node_id,
			//lemon::SmartGraph::NodeMap<double>& y, lemon::SmartGraph::NodeMap<double>& Ei);
};

// ========================== //

#endif // SPPM_GEOJSON_READER_H_
