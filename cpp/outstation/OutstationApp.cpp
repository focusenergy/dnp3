/**
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <thread>

#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/PrintingSOEHandler.h>
#include <asiodnp3/ConsoleLogger.h>
#include <asiopal/UTCTimeSource.h>
#include <opendnp3/outstation/Database.h>
#include <opendnp3/LogLevels.h>

#include <boost/program_options.hpp>

#include <yaml-cpp/yaml.h>

#include "AsyncCommandHandler.cpp"
#include "OutstationJSONTCPServer.cpp"

namespace po = boost::program_options;

using namespace opendnp3;
using namespace openpal;

std::condition_variable g_signal_cond;
std::mutex g_signal_mutex;
volatile std::sig_atomic_t g_signal;

void signal_handler(int signal_number) {
	std::unique_lock<std::mutex> lock(g_signal_mutex);
	g_signal = signal_number;
	g_signal_cond.notify_all();
}

/**
 * Outstation Application providing secondary control channel over TCP (JSON payload).
 *
 * See OutstationJSONTCPServer for details of secondary control channel.
 */
class OutstationApp {
public:
	/**
	 * Validates that config contains needed elements for basic configuration.
	 * Does not validate correctness of config elements.
	 */
	static bool validConfig(YAML::Node& config) {
		bool valid = config["master"] && config["master"]["id"] && config["master"]["host"] && config["master"]["port"] && config["outstations"]
				&& config["outstations"][0] && config["outstations"][0]["id"] && config["outstations"][0]["localAddr"]
				&& config["outstations"][0]["remoteAddr"];
		if (!valid)
			std::cerr << "Invalid YAML config, check master (id, host, port) and ou tstations (id)" << std::endl;
		return valid;
	}

	void start(YAML::Node& config) {
		DNP3Manager manager(1);
		manager.AddLogSubscriber(&ConsoleLogger::Instance());

		IChannel* pChannel = configureManagerMaster(config, manager);
		pChannel->AddStateListener([](ChannelState state)
		{
			std::cout << "channel state: " << ChannelStateToString(state) << std::endl;
		});

		AsyncCommandHandler handler;
		/** TODO for each outstation, configure **/
		for (std::size_t i = 0; i < config["outstations"].size(); i++) {
			YAML::Node outstationConfig = config["outstations"][i];
			IOutstation* pOutstation = configureOutstation(outstationConfig, pChannel, handler);

			/** TODO Enable all at end of loop, keep separate list for OJTS, add multi-outstation handling **/
			pOutstation->Enable();
			OutstationJSONTCPServer s(io_service_, 3384, pOutstation, handler);
			io_service_.run();
		}

	}

	void stop() {
		// TODO shutdown correctly, consider DNP3Manager.shutdown()
		io_service_.stop();
	}
private:
	IChannel* configureManagerMaster(YAML::Node config, DNP3Manager& manager) {
		return manager.AddTCPServer("server", levels::NORMAL, ChannelRetry::Default(), "0.0.0.0", 20000);
	}

	IOutstation* configureOutstation(YAML::Node& outConf, IChannel* pChannel, AsyncCommandHandler& handler) {
		OutstationStackConfig stackConfig;
		stackConfig.link.LocalAddr = outConf["localAddr"].as<int>();
		stackConfig.link.RemoteAddr = outConf["remoteAddr"].as<int>();

		if (outConf["measDB"]) {
			int binaries = outConf["measDB"]["binaries"].size();
			int doubleBinaries = outConf["measDB"]["doubleBinaries"].size();
			int analogs = outConf["measDB"]["analogs"].size();
			int counters = outConf["measDB"]["counters"].size();
			int frozenCounters = outConf["measDB"]["frozenCounters"].size();
			int binaryOutputStatii = outConf["measDB"]["binaryOutputStatii"].size();
			int analogOutputStatii = outConf["measDB"]["analogOutputStatii"].size();
			int timeAndIntervals = outConf["measDB"]["timeAndIntervals"].size();

			stackConfig.dbTemplate = DatabaseTemplate(binaries, doubleBinaries, analogs, counters, frozenCounters, binaryOutputStatii, analogOutputStatii,
					timeAndIntervals);
			stackConfig.outstation.eventBufferConfig = EventBufferConfig(binaries, doubleBinaries, analogs, counters, frozenCounters, binaryOutputStatii,
					analogOutputStatii, timeAndIntervals);
		} else {
			/** using default demo configuration **/
			stackConfig.dbTemplate = DatabaseTemplate::AllTypes(5);
			stackConfig.outstation.eventBufferConfig = EventBufferConfig::AllTypes(5);
		}

		IOutstation* pOutstation = pChannel->AddOutstation(((std::string) outConf["id"].as<std::string>()).c_str(), handler,
				DefaultOutstationApplication::Instance(), stackConfig);

		DatabaseConfigView view = pOutstation->GetConfigView();
		// TODO read from config file
		view.analogs[0].variation = StaticAnalogVariation::Group30Var5;
		view.analogs[0].metadata.clazz = PointClass::Class2;
		view.analogs[0].metadata.variation = EventAnalogVariation::Group32Var7;

		return pOutstation;
	}

	boost::asio::io_service io_service_;
};

int main(int argc, char* argv[]) {
	std::string confFile;/** "cpp/test/outstation/demo.yml" **/

	std::signal(SIGINT, signal_handler);

	po::options_description desc("Options");
	desc.add_options()("conf", po::value<std::string>(&confFile)->required(), "yaml configuration file")("help", "show help");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cerr << desc << std::endl;
		return 1;
	}

	YAML::Node config = YAML::LoadFile(confFile);
	if (!OutstationApp::validConfig(config))
		return 1;

	OutstationApp app;
	std::thread t([&app, &config] {app.start(config);});
	t.detach();

	std::unique_lock<std::mutex> lock(g_signal_mutex);
	while (!g_signal) {
		g_signal_cond.wait(lock);
	}
	app.stop();
	return 0;
}
