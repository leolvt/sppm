/*
   Copyright (C) 2014  Leonardo Vilela Teixeira

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
   */

#include <stdexcept>
#include <string>

#include "easylogging++.h"
#include <gflags/gflags.h>
#include <lemon/smart_graph.h>

#include "geojson_reader.h"
#include "sppm_normal.h"
#include "sppm_poisson.h"
#include "util.h"

using namespace std;

// ========================== //

DEFINE_uint64(num_iter, 1000, "number of iterations to run");
DEFINE_uint64(burn_in, 100, "burn-in period");
DEFINE_uint64(thinning, 10, "thinning. Take only each i-th sampled value");
DEFINE_uint64(verbose, 9, "verbose level");
//DEFINE_string(output_dir, ".", "directory where the output CSV files will "
	//"be saved");

static const char USAGE[] =
R"(
Usage:
	sppm [options] [--] normal <GeoJSON> <attr> <r> <s> <m> <v> <a> <b>
	sppm [options] [--] poisson <GeoJSON> <attr_Yi> <attr_Ei> <r> <s> <a> <b>
)";

INITIALIZE_EASYLOGGINGPP

// ========================== //

int main(int argc, char* argv[])
{
	//START_EASYLOGGINGPP(argc, argv);
	gflags::SetUsageMessage(USAGE);
	gflags::SetVersionString("Spatial PPM Version 1.0");
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	// Load logger configuration
	el::Configurations defaultConf;
	defaultConf.set(el::Level::Verbose, el::ConfigurationType::ToStandardOutput, "false");
	el::Loggers::reconfigureAllLoggers(defaultConf);

	el::Configurations conf("default-logger.conf");
	el::Loggers::reconfigureAllLoggers(conf);
	if (FLAGS_verbose != 0) {
		el::Loggers::setVerboseLevel(FLAGS_verbose);
	}

	// Running parameters
	int num_iter = FLAGS_num_iter;
	int burn_in = FLAGS_burn_in;
	int steps = FLAGS_thinning;

	try {
		lemon::SmartGraph graph;
		lemon::SmartGraph::NodeMap<long long> node_id(graph);
		lemon::SmartGraph::NodeMap<Util::AttrMap> node_attribute(graph);

		// Run the normal case
		if (argc == 10 && string(argv[1]) == "normal") {

			// Parse the arguments
			string input_file = argv[2];
			string attr = argv[3];
			double r = stod(argv[4]);
			double s = stod(argv[5]);
			double m = stod(argv[6]);
			double v = stod(argv[7]);
			double a = stod(argv[8]);
			double b = stod(argv[9]);

			// Read the data
			GeoJSONReader reader;
			bool ok = reader.LoadData(input_file, graph, node_id, node_attribute);
			if (!ok) {
				LOG(FATAL) << "Failed to load data from file: " << input_file;
			}

			// Set up the algorithm with the given parameters
			LOG(INFO) << "Using attribute: Yi = "+ attr;
			SPPM_Normal sppm(graph, node_id, node_attribute);
			sppm.SetRhoParameters(r, s);
			//sppm.SetRhoParameters(2, 850);
			//sppm.SetRhoParameters(5, 5500);
			//sppm.SetRhoParameters(1000, 1100000);
			//sppm.SetRhoParameters(52, 5450);
			//sppm.SetNormalGammaParameters(40, 0.1, 0.65, 0.04);
			//sppm.SetNormalGammaParameters(100, 1, 0.65, 0.04);
			//sppm.SetNormalGammaParameters(400, 1, 0.65, 0.04);
			//sppm.SetNormalGammaParameters(400, 1, 0.65, 0.04);
			//sppm.SetNormalGammaParameters(1600, 1, 0.65, 0.01);
			//sppm.SetNormalGammaParameters(40000, 49, 0.65, 0.0196);
			//sppm.SetNormalGammaParameters(10000, 64, 0.65, 0.01024);
			sppm.SetNormalGammaParameters(a, b, m, v);
			sppm.SetAttribute(attr);

			// Run the algorithm
			sppm.Run(num_iter, burn_in, steps);

		// Run the poisson case
		} else if (argc == 9 && string(argv[1]) == "poisson") {
			string input_file = argv[2];
			string attr_Yi = argv[3];
			string attr_Ei = argv[4];
			double r = stod(argv[5]);
			double s = stod(argv[6]);
			double a = stod(argv[7]);
			double b = stod(argv[8]);

			// Read the data
			GeoJSONReader reader;
			bool ok = reader.LoadData(input_file, graph, node_id, node_attribute);
			if (!ok) {
				LOG(FATAL) << "Failed to load data from file: " << input_file;
			}

			LOG(INFO) << "Using attributes: Yi = "+ attr_Yi + ", Ei = " + attr_Ei;
			SPPM_Poisson sppm(graph, node_id, node_attribute);
			sppm.SetRhoParameters(r, s);
			sppm.SetGammaParameters(a, b);
			sppm.SetAttributes(attr_Yi, attr_Ei);

			// Run the algorithm
			sppm.Run(num_iter, burn_in, steps);

		// Invalid case
		} else {
			cerr << "Invalid usage." << endl;
			cerr << USAGE << endl;
			return 1;
		}
	} catch (const char* e) {
		LOG(FATAL) << "Exception caught: " << e;
	} catch (std::invalid_argument) {
		LOG(FATAL) << "Invalid arguments. Conversion failed.";
	} catch (std::exception e) {
		LOG(FATAL) << "Exception caught: " << e.what();
	} catch (...) {
		LOG(FATAL) << "Unknown exception! ";
	}

	return 0;
}
