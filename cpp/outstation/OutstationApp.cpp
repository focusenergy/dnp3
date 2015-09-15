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
#include <asiopal/UTCTimeSource.h>
#include <cpp/outstation/AsyncCommandHandler.h>
#include <cpp/outstation/OutstationJSONTCPServer.cpp>
#include <opendnp3/outstation/Database.h>
#include <opendnp3/LogLevels.h>

#include <iostream>
#include <thread>

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
	void start() {
		DNP3Manager manager(1);
		manager.AddLogSubscriber(&ConsoleLogger::Instance());

		IChannel* pChannel = manager.AddTCPServer("server", levels::NORMAL, TimeDuration::Seconds(5), TimeDuration::Seconds(5), "0.0.0.0", 20000);
		pChannel->AddStateListener([](ChannelState state)
		{
			std::cout << "channel state: " << ChannelStateToString(state) << std::endl;
		});

		OutstationStackConfig stackConfig;
		stackConfig.dbTemplate = DatabaseTemplate::AllTypes(5);
		stackConfig.outstation.eventBufferConfig = EventBufferConfig::AllTypes(5);
		stackConfig.link.LocalAddr = 10;
		stackConfig.link.RemoteAddr = 1;

		AsyncCommandHandler handler;
		IOutstation* pOutstation = pChannel->AddOutstation("outstation", handler, DefaultOutstationApplication::Instance(), stackConfig);

		// Configure and start outstation
		ConfigureDatabase(pOutstation->GetConfigView());
		pOutstation->Enable();

		OutstationJSONTCPServer s(io_service_, 3384, pOutstation, handler);
		io_service_.run();
	}

	void stop() {
		// TODO shutdown correctly
		io_service_.stop();
	}
private:
	void ConfigureDatabase(DatabaseConfigView view) {
		// TODO read from config file
		view.analogs[0].variation = StaticAnalogVariation::Group30Var5;
		view.analogs[0].metadata.clazz = PointClass::Class2;
		view.analogs[0].metadata.variation = EventAnalogVariation::Group32Var7;
	}

	boost::asio::io_service io_service_;
};

int main(int argc, char* argv[]) {
	std::signal(SIGINT, signal_handler);

	OutstationApp app;
	std::thread t([&app] {app.start();});
	t.detach();

	std::unique_lock<std::mutex> lock(g_signal_mutex);
	while (!g_signal) {
		g_signal_cond.wait(lock);
	}
	app.stop();
	return 0;
}
