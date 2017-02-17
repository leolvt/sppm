#include "geojson_reader.h"

#include <cstdio>
#include <unordered_map>

#include "easylogging++.h"

#include <lemon/smart_graph.h>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

using namespace std;
using namespace lemon;
using namespace rapidjson;

// ========================== //

bool GeoJSONReader::LoadData(string filename, SmartGraph& graph,
	SmartGraph::NodeMap<long long>& node_id,
	SmartGraph::NodeMap<Util::AttrMap>& node_attribute) {

	LOG(INFO) << "Reading data from GeoJSON (file: " + filename + ")";

	// TODO: Handle failure when opening file
	FILE* fp = fopen(filename.c_str(), "r");
	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document d;
	d.ParseStream(is);
	fclose(fp);

	// Store the nodes for each ID. Needed to build the edges.
	unordered_map<int, SmartGraph::Node> id2nodes;

	// Add the nodes from the GeoJSON data to the graph
	int node_count = 0;
	for (auto itr = d["features"].Begin(); itr != d["features"].End(); ++itr) {

		if (!itr->HasMember("id") || ! (*itr)["id"].IsNumber()) {
			cerr << "Failed reading data!. Found object without int ID." << endl;
			return false;
		}

		// Get the node ID
		long long u_id = (*itr)["id"].GetInt64();

		// Add the node to the graph
		SmartGraph::Node u = graph.addNode();
		id2nodes[u_id] = u;
		node_id[u] = u_id;
		node_count++;

		// Read the attributes
		//static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
		auto mbr_itr = (*itr)["properties"].MemberBegin();
		for (; mbr_itr != (*itr)["properties"].MemberEnd(); ++mbr_itr) {
			// We only use numeric attributes
			if (mbr_itr->value.GetType() == 6){
				if (mbr_itr->value.IsInt()) {
					node_attribute[u][mbr_itr->name.GetString()] = mbr_itr->value.GetDouble();
					//cerr << mbr_itr->name.GetString() << " -> " << mbr_itr->value.GetDouble() << endl;
				}
				else if (mbr_itr->value.IsDouble()) {
					node_attribute[u][mbr_itr->name.GetString()] = mbr_itr->value.GetDouble();
					//cerr << mbr_itr->name.GetString() << " -> " << mbr_itr->value.GetDouble() << endl;
				}
			}
		}
	}

	// Add the edges from the GeoJSON data to the graph
	int edge_count = 0;
	for (auto itr = d["features"].Begin(); itr != d["features"].End(); ++itr) {

		// Get the current node
		int u_id = (*itr)["id"].GetInt();
		SmartGraph::Node u = id2nodes[u_id];

		// TODO: Check if the 'neighbours' list is there
		Value& nb_list = (*itr)["neighbours"];
		for (auto nb_itr = nb_list.Begin(); nb_itr != nb_list.End(); ++nb_itr) {
			// TODO: Check if it is indeed a number
			int v_id = nb_itr->GetInt();

			// Check if this node is in our hash
			if (id2nodes.find(v_id) == id2nodes.end()) {
				cerr << "Neighbour node not found: " << u_id << " -> " << v_id << endl;
				return false;
			}

			// Retrieve the nodes and add the edge
			if (u_id < v_id) {
				SmartGraph::Node v = id2nodes[v_id];
				graph.addEdge(u, v);
				edge_count++;
			}
		}
	}

	string added_msg = "Read " + to_string(node_count) + " nodes and ";
	added_msg += to_string(edge_count) + " edges.";
	LOG(INFO) << added_msg;

	return true;
}

// ========================== //

#if 0
bool GeoJSONReader::LoadData(string filename, string attribute,
	SmartGraph& graph, SmartGraph::NodeMap<int>& node_id,
	SmartGraph::NodeMap<double>& y) {

	LOG(INFO) << "Reading data from GeoJSON (file: " + filename + ")";
	LOG(INFO) << "Using attribute: "+ attribute;

	// TODO: Handle failure
	FILE* fp = fopen(filename.c_str(), "r");
	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document d;
	d.ParseStream(is);
	fclose(fp);

	// Store the nodes for each ID. Needed to build the edges.
	unordered_map<int, SmartGraph::Node> id2nodes;

	// Add the nodes from the GeoJSON data to the graph
	int node_count = 0;
	for (auto itr = d["features"].Begin(); itr != d["features"].End(); ++itr) {

		if (!itr->HasMember("id") || ! (*itr)["id"].IsNumber()) {
			cerr << "Failed reading data!. Found object without int ID." << endl;
			return false;
		}

		// TODO: Check if attribute is valid
		double y_val = (*itr)["properties"][attribute.c_str()].GetDouble();
		SmartGraph::Node u = graph.addNode();
		y[u] = y_val;
		node_count++;

		// Add node to the hash
		int u_id = (*itr)["id"].GetInt();
		id2nodes[u_id] = u;
		node_id[u] = u_id;
	}

	// Add the edges from the GeoJSON data to the graph
	int edge_count = 0;
	for (auto itr = d["features"].Begin(); itr != d["features"].End(); ++itr) {

		// Get the current node
		int u_id = (*itr)["id"].GetInt();
		SmartGraph::Node u = id2nodes[u_id];

		// TODO: Check if the 'neighbours' list is there
		Value& nb_list = (*itr)["neighbours"];
		for (auto nb_itr = nb_list.Begin(); nb_itr != nb_list.End(); ++nb_itr) {
			// TODO: Check if it is indeed a number
			int v_id = nb_itr->GetInt();

			// Check if this node is in our hash
			if (id2nodes.find(v_id) == id2nodes.end()) {
				cerr << "Neighbour node not found: " << u_id << " -> " << v_id << endl;
				return false;
			}

			// Retrieve the nodes and add the edge
			if (u_id < v_id) {
				SmartGraph::Node v = id2nodes[v_id];
				graph.addEdge(u, v);
				edge_count++;
			}
		}
	}

	string added_msg = "Read " + to_string(node_count) + " nodes and ";
	added_msg += to_string(edge_count) + " edges.";
	LOG(INFO) << added_msg;

	return true;
}

// ========================== //

bool GeoJSONReader::LoadData(string filename, string attribute,
	SmartGraph& graph, SmartGraph::NodeMap<int>& node_id,
	SmartGraph::NodeMap<double>& y, SmartGraph::NodeMap<double>& Ei) {

	LOG(INFO) << "Reading data from GeoJSON (file: " + filename + ")";
	LOG(INFO) << "Using attribute: "+ attribute;

	// TODO: Handle failure
	FILE* fp = fopen(filename.c_str(), "r");
	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document d;
	d.ParseStream(is);
	fclose(fp);

	// Store the nodes for each ID. Needed to build the edges.
	unordered_map<int, SmartGraph::Node> id2nodes;

	// Add the nodes from the GeoJSON data to the graph
	int node_count = 0;
	for (auto itr = d["features"].Begin(); itr != d["features"].End(); ++itr) {

		if (!itr->HasMember("id") || ! (*itr)["id"].IsNumber()) {
			cerr << "Failed reading data!. Found object without int ID." << endl;
			return false;
		}

		// TODO: Check if attribute is valid
		double y_val = (*itr)["properties"][attribute.c_str()].GetDouble();
		SmartGraph::Node u = graph.addNode();
		y[u] = y_val;
		double ei_val = (*itr)["properties"]["EI"].GetDouble();
		Ei[u] = ei_val;
		node_count++;

		// Add node to the hash
		int u_id = (*itr)["id"].GetInt();
		id2nodes[u_id] = u;
		node_id[u] = u_id;
	}

	// Add the edges from the GeoJSON data to the graph
	int edge_count = 0;
	for (auto itr = d["features"].Begin(); itr != d["features"].End(); ++itr) {

		// Get the current node
		int u_id = (*itr)["id"].GetInt();
		SmartGraph::Node u = id2nodes[u_id];

		// TODO: Check if the 'neighbours' list is there
		Value& nb_list = (*itr)["neighbours"];
		for (auto nb_itr = nb_list.Begin(); nb_itr != nb_list.End(); ++nb_itr) {
			// TODO: Check if it is indeed a number
			int v_id = nb_itr->GetInt();

			// Check if this node is in our hash
			if (id2nodes.find(v_id) == id2nodes.end()) {
				cerr << "Neighbour node not found: " << u_id << " -> " << v_id << endl;
				return false;
			}

			// Retrieve the nodes and add the edge
			if (u_id < v_id) {
				SmartGraph::Node v = id2nodes[v_id];
				graph.addEdge(u, v);
				edge_count++;
			}
		}
	}

	string added_msg = "Read " + to_string(node_count) + " nodes and ";
	added_msg += to_string(edge_count) + " edges.";
	LOG(INFO) << added_msg;

	return true;
}
#endif

// ========================== //
