#include <iostream>

using namespace asiodnp3;

class OutstationAppConfig {

public:
	OutstationAppConfig(std::string confFile) {
		configNode_ = YAML::LoadFile(confFile);
	}

	/**
	 * Validates that config contains needed elements for basic configuration.
	 * Does not validate correctness of config elements.
	 */
	bool isValid() {
		bool valid = configNode_["master"] && configNode_["master"]["id"] && configNode_["master"]["host"] && configNode_["master"]["port"]
				&& configNode_["outstations"] && configNode_["outstations"][0] && configNode_["outstations"][0]["id"]
				&& configNode_["outstations"][0]["localAddr"] && configNode_["outstations"][0]["remoteAddr"];
		if (!valid)
			std::cerr << "Invalid YAML config, check master (id, host, port) and outstations (id)" << std::endl;
		return valid;
	}

	const char* getMasterId() {
		return ((std::string) configNode_["master"]["id"].as<std::string>()).c_str();
	}

	const char* getMasterHost() {
		return ((std::string) configNode_["master"]["host"].as<std::string>()).c_str();
	}

	int getMasterPort() {
		return configNode_["master"]["port"].as<int>();
	}

	int getOutstationsCount() {
		return configNode_["outstations"].size();
	}

	std::string getOutstationId(int idx) {
		return configNode_["outstations"][idx]["id"].as<std::string>();
	}

	IOutstation* configureOutstation(int idx, IChannel* pChannel, AsyncCommandQueue& queue) {
		/** TODO consider decoupling dnp3 from configuration class **/
		YAML::Node outstationNode = configNode_["outstations"][idx];
		int binaries, doubleBinaries, analogs, counters, frozenCounters, binaryOutputStatii, analogOutputStatii, timeAndIntervals;
		if (configNode_["measDB"]) {
			binaries = configNode_["measDB"]["binaries"].size();
			doubleBinaries = configNode_["measDB"]["doubleBinaries"].size();
			analogs = configNode_["measDB"]["analogs"].size();
			counters = configNode_["measDB"]["counters"].size();
			frozenCounters = configNode_["measDB"]["frozenCounters"].size();
			binaryOutputStatii = configNode_["measDB"]["binaryOutputStatii"].size();
			analogOutputStatii = configNode_["measDB"]["analogOutputStatii"].size();
			timeAndIntervals = configNode_["measDB"]["timeAndIntervals"].size();
		} else {
			/** using default demo configuration DatabaseTemplate::AllTypes(10), EventBufferConfig::AllTypes(10); **/
			binaries = doubleBinaries = analogs = counters = frozenCounters = binaryOutputStatii = analogOutputStatii = timeAndIntervals = 10;
		}
		const char* outstationId = outstationNode["id"].as<std::string>().c_str();
		AsyncCommandHandler* handler = new AsyncCommandHandler(outstationId, queue);
		IOutstation* pOutstation = pChannel->AddOutstation(outstationId, *handler, DefaultOutstationApplication::Instance(),
				createStackConfig(outstationNode, binaries, doubleBinaries, analogs, counters, frozenCounters, binaryOutputStatii, analogOutputStatii,
						timeAndIntervals));
		configureDatabaseConfigView(pOutstation->GetConfigView(), binaries, doubleBinaries, analogs, counters, frozenCounters, binaryOutputStatii,
				analogOutputStatii, timeAndIntervals);
		return pOutstation;
	}

private:
	OutstationStackConfig createStackConfig(YAML::Node& outstationNode, int binaries, int doubleBinaries, int analogs, int counters, int frozenCounters,
			int binaryOutputStatii, int analogOutputStatii, int timeAndIntervals) {
		OutstationStackConfig stackConfig;
		stackConfig.link.LocalAddr = outstationNode["localAddr"].as<int>();
		stackConfig.link.RemoteAddr = outstationNode["remoteAddr"].as<int>();
		stackConfig.dbTemplate = DatabaseTemplate(binaries, doubleBinaries, analogs, counters, frozenCounters, binaryOutputStatii, analogOutputStatii,
				timeAndIntervals);
		stackConfig.outstation.eventBufferConfig = EventBufferConfig(binaries, doubleBinaries, analogs, counters, frozenCounters, binaryOutputStatii,
				analogOutputStatii, timeAndIntervals);
		return stackConfig;
	}

	void configureDatabaseConfigView(DatabaseConfigView view, int binaries, int doubleBinaries, int analogs, int counters, int frozenCounters,
			int binaryOutputStatii, int analogOutputStatii, int timeAndIntervals) {
		for (int i = 0; i < binaries; i++) {

		}
		for (int i = 0; i < doubleBinaries; i++) {

		}
		for (int i = 0; i < analogs; i++) {

		}
		for (int i = 0; i < counters; i++) {

		}
		for (int i = 0; i < frozenCounters; i++) {

		}
		for (int i = 0; i < binaryOutputStatii; i++) {

		}
		for (int i = 0; i < analogOutputStatii; i++) {

		}
		for (int i = 0; i < timeAndIntervals; i++) {

		}
	}

	YAML::Node configNode_;
};
