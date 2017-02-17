#include "simple_reader.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include <lemon/smart_graph.h>

using namespace std;
using namespace lemon;

// ========================== //

// TODO: Check errors, etc.
bool SimpleReader::LoadData(string filename, string attribute,
	SmartGraph& graph, SmartGraph::NodeMap<int>& node_id,
	SmartGraph::NodeMap<double>& y) {

	// We are going to need a map id -> node
	unordered_map<int, SmartGraph::Node> id2node;

	// Set filenames
	size_t ext_pos = filename.find_last_of(".") + 1;
	string val_filename = filename;
	string adj_filename = filename;
	val_filename.replace(ext_pos, string::npos, "val");
	adj_filename.replace(ext_pos, string::npos, "adj");

	// Open Nodes file
	ifstream val_file(val_filename, fstream::in);

	//Store the attribute headers (name & type) in vectors
	vector<string> attr_name;
	vector<string> attr_type;
	int target_attr = -1;

	// First, read the two lines (name and type)
	string headers_names, headers_types;
	getline(val_file, headers_names);
	getline(val_file, headers_types);

	// Now, split the lines and fill the vectors
	stringstream ss_names(headers_names);
	stringstream ss_types(headers_types);
	string attr, type;
	int idx = 0;

	// Keep reading the attributes
	while (getline(ss_names, attr, ' ')) {
		getline(ss_types, type, ' ');

		// Store the name and type
		attr_name.push_back(attr);
		attr_type.push_back(type);

		// Check if this is the attribute we want to use
		if (attr == attribute) {
			target_attr = idx;
		}

		++idx;
	}

	// Read the nodes,one line at a time (each line is a node)
	string line;
	while (getline(val_file, line)) {

		// We use this to parse the attributes
		stringstream ss(line);

		// Read the node ID and number of attributes
		int u_id;
		int num_attr;
		ss >> u_id;
		ss >> num_attr;

		// Read the attributes and keep the one of interest
		double y_val = 0.0;
		string tmp;
		int idx = 0;
		while (num_attr-- > 0) {
			// We need only the target attribute, so it is safe to stop here
			if (idx == target_attr) {
				ss >> y_val;
				break;
			}
			// Until we get to our target, keep skipping the attributes
			else {
				ss >> tmp;
			}
			++idx;
		}

		// Add the node and the attribute
		SmartGraph::Node u = graph.addNode();
		y[u] = y_val;
		node_id[u] = u_id;
		id2node[u_id] = u;
	}

	// Finished the '.val' file, can close it
	val_file.close();


	// Open Edges file
	ifstream adj_file(adj_filename, fstream::in);

	// Read Edges
	while (getline(adj_file, line)) {
		stringstream ss(line);
		int u_id, v_id;
		int num_neighbours;

		ss >> u_id;
		ss >> num_neighbours;

		while (num_neighbours > 0) {
			ss >> v_id;
			SmartGraph::Node u = id2node[u_id];
			SmartGraph::Node v = id2node[v_id];
			graph.addEdge(u, v);
			num_neighbours--;
		}

	}

	cout << "Added " << countNodes(graph) << " nodes and ";
	cout << countEdges(graph) << " edges" << endl;

	return true;
}

// ========================== //

