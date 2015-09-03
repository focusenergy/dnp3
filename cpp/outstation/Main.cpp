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

#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/PrintingSOEHandler.h>
#include <asiodnp3/ConsoleLogger.h>
#include <asiodnp3/MeasUpdate.h>

#include <asiopal/UTCTimeSource.h>
#include <cpp/outstation/AsyncCommandHandler.h>
#include <cpp/outstation/OutstationJSONTCPServer.cpp>
#include <opendnp3/outstation/Database.h>
#include <opendnp3/LogLevels.h>

#include <string>
#include <thread>
#include <iostream>
#include <csignal>
#include <unistd.h>


using namespace std;
using namespace opendnp3;
using namespace openpal;
using namespace asiopal;
using namespace asiodnp3;

namespace {
volatile std::sig_atomic_t gSignalStatus;
}

void signal_handler(int signal) {
	gSignalStatus = signal;
}

void ConfigureDatabase(DatabaseConfigView view) {
	// TODO read from config file
	view.analogs[0].variation = StaticAnalogVariation::Group30Var5;
	view.analogs[0].metadata.clazz = PointClass::Class2;
	view.analogs[0].metadata.variation = EventAnalogVariation::Group32Var7;
}

int main(int argc, char* argv[]) {
	DNP3Manager manager(1);
	manager.AddLogSubscriber(&ConsoleLogger::Instance());

	auto pChannel = manager.AddTCPServer("server", levels::NORMAL, TimeDuration::Seconds(5), TimeDuration::Seconds(5), "0.0.0.0", 20000);
	pChannel->AddStateListener([](ChannelState state)
	{
		std::cout << "channel state: " << ChannelStateToString(state) << std::endl;
	});

	OutstationStackConfig stackConfig;
	stackConfig.dbTemplate = DatabaseTemplate::AllTypes(5);
	stackConfig.outstation.eventBufferConfig = EventBufferConfig::AllTypes(5);
	stackConfig.link.LocalAddr = 10;
	stackConfig.link.RemoteAddr = 1;

	// TODO configure and run (tcp or other) server for JSON commands/events
	// 1. convert incoming JSON to MeasUpdate and apply to outstation instance
	// 2. JSONCommandHandler incoming DNP3 commands should convert and forward via JSON

	AsyncCommandHandler handler;
	IOutstation* pOutstation = pChannel->AddOutstation("outstation", handler, DefaultOutstationApplication::Instance(), stackConfig);

	// Configure and start outstation
	ConfigureDatabase(pOutstation->GetConfigView());
	pOutstation->Enable();

	try {
		boost::asio::io_service io_service;
		OutstationJSONTCPServer s(io_service, 3384, pOutstation);
		io_service.run();

		// Initiate shutdown when signal received
		std::signal(SIGINT, signal_handler);
		pause();

		io_service.stop();
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	pOutstation->Shutdown();
	manager.Shutdown();
	return 0;
}
