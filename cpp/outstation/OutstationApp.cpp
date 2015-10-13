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
#include <boost/signals2.hpp>

#include <yaml-cpp/yaml.h>

#include "AsyncCommandQueue.cpp"
#include "AsyncCommandHandler.cpp"
#include "OutstationAppConfig.cpp"
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
	void start(OutstationAppConfig& config) {
		DNP3Manager manager(1);
		manager.AddLogSubscriber(&ConsoleLogger::Instance());

		IChannel* pChannel = manager.AddTCPServer(config.getMasterId(), levels::NORMAL, ChannelRetry::Default(), config.getMasterHost(),
				config.getMasterPort());
		pChannel->AddStateListener([](ChannelState state)
		{
			std::cout << "channel state: " << ChannelStateToString(state) << std::endl;
		});

		AsyncCommandQueue commandQueue;
		std::map<std::string, IOutstation*> outstations;
		for (int i = 0; i < config.getOutstationsCount(); i++) {
			outstations.insert(std::pair<std::string, IOutstation*>(config.getOutstationId(i), config.configureOutstation(i, pChannel, commandQueue)));
		}

		/** configurations were successful, start all outstations */
		for (std::pair<std::string, IOutstation*> outstation : outstations) {
			outstation.second->Enable();
		}

		OutstationJSONTCPServer s(io_service_, 3384, outstations, commandQueue);
		io_service_.run();

	}

	void stop() {
		// TODO shutdown correctly, consider DNP3Manager.shutdown()
		io_service_.stop();
	}
private:
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

	OutstationAppConfig appConfig(confFile);
	if (!appConfig.isValid())
		return 1;

	OutstationApp app;
	std::thread t([&app, &appConfig] {app.start(appConfig);});
	t.detach();

	std::unique_lock<std::mutex> lock(g_signal_mutex);
	while (!g_signal) {
		g_signal_cond.wait(lock);
	}
	app.stop();
	return 0;
}
