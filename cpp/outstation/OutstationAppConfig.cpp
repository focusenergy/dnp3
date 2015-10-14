#include <iostream>

#include "opendnp3/app/MeasurementTypes.h"

using namespace asiodnp3;

/**
 * Class responsible for parsing OutstationApp YAML Configuration
 * and configuring appropriate OpenDNP3 Outstation objects.
 *
 * TODO implement a better String<->Enum mapping technique for variations/metadata/eventVariations
 */
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
		configureDatabaseConfigView(outstationNode, pOutstation->GetConfigView());
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

	void configureDatabaseConfigView(YAML::Node& outstationNode, DatabaseConfigView view) {
		if (outstationNode["measDB"]) {
			configureBinaries(outstationNode, view.binaries);
			configureDoubleBinaries(outstationNode, view.doubleBinaries);
			configureAnalogs(outstationNode, view.analogs);
			configureCounters(outstationNode, view.counters);
			configureFrozenCounters(outstationNode, view.frozenCounters);
			configureBinaryOutputStatii(outstationNode, view.binaryOutputStatii);
			configureAnalogOutputStatii(outstationNode, view.analogOutputStatii);
			configureTimeAndIntervals(outstationNode, view.timeAndIntervals);
		}
	}

	void configureBinaries(YAML::Node& outstationNode, openpal::ArrayView<Cell<Binary>, uint16_t>& binaries) {
		const char* type = "binaries";
		for (int i = 0; i < binaries.Size(); i++) {
			if (hasVariation(outstationNode, type, i)) {
				std::string variation = getVariation(outstationNode, type, i);
				if (variation == "StaticBinaryVariation::Group1Var1") {
					binaries[i].variation = StaticBinaryVariation::Group1Var1;
				} else if (variation == "StaticBinaryVariation::Group1Var2") {
					binaries[i].variation = StaticBinaryVariation::Group1Var2;
				}
			}
			if (hasMetadata(outstationNode, type, i)) {
				if (hasMetadataClazz(outstationNode, type, i)) {
					binaries[i].metadata.clazz = getPointClass(getMetadataClazz(outstationNode, type, i));
				}
				if (hasMetadataVariation(outstationNode, type, i)) {
					std::string metadataVariation = getMetadataVariation(outstationNode, type, i);
					if (metadataVariation == "EventBinaryVariation::Group2Var1") {
						binaries[i].metadata.variation = EventBinaryVariation::Group2Var1;
					} else if (metadataVariation == "EventBinaryVariation::Group2Var2") {
						binaries[i].metadata.variation = EventBinaryVariation::Group2Var2;
					} else if (metadataVariation == "EventBinaryVariation::Group2Var3") {
						binaries[i].metadata.variation = EventBinaryVariation::Group2Var3;
					} else {
						std::cerr << "OutstationAppConfig: unable to map metadataVariation[" << metadataVariation << "]" << std::endl;
					}
				}
			}
		}
	}

	void configureDoubleBinaries(YAML::Node& outstationNode, openpal::ArrayView<Cell<DoubleBitBinary>, uint16_t>& doubleBinaries) {
		const char* type = "doubleBinaries";
		for (int i = 0; i < doubleBinaries.Size(); i++) {
			if (hasVariation(outstationNode, type, i)) {
				std::string variation = getVariation(outstationNode, type, i);
				if (variation == "StaticDoubleBinaryVariation::Group3Var2") {
					doubleBinaries[i].variation = StaticDoubleBinaryVariation::Group3Var2;
				}
			}
			if (hasMetadata(outstationNode, type, i)) {
				if (hasMetadataClazz(outstationNode, type, i)) {
					doubleBinaries[i].metadata.clazz = getPointClass(getMetadataClazz(outstationNode, type, i));
				}
				if (hasMetadataVariation(outstationNode, type, i)) {
					std::string metadataVariation = getMetadataVariation(outstationNode, type, i);
					if (metadataVariation == "EventDoubleBinaryVariation::Group4Var1") {
						doubleBinaries[i].metadata.variation = EventDoubleBinaryVariation::Group4Var1;
					} else if (metadataVariation == "EventDoubleBinaryVariation::Group4Var2") {
						doubleBinaries[i].metadata.variation = EventDoubleBinaryVariation::Group4Var2;
					} else if (metadataVariation == "EventDoubleBinaryVariation::Group4Var3") {
						doubleBinaries[i].metadata.variation = EventDoubleBinaryVariation::Group4Var3;
					} else {
						std::cerr << "OutstationAppConfig: unable to map metadataVariation[" << metadataVariation << "]" << std::endl;
					}
				}
			}
		}
	}

	void configureAnalogs(YAML::Node& outstationNode, openpal::ArrayView<Cell<Analog>, uint16_t>& analogs) {
		const char* type = "analogs";
		for (int i = 0; i < analogs.Size(); i++) {
			if (hasVariation(outstationNode, type, i)) {
				std::string variation = getVariation(outstationNode, type, i);
				if (variation == "StaticAnalogVariation::Group30Var1") {
					analogs[i].variation = StaticAnalogVariation::Group30Var1;
				} else if (variation == "StaticAnalogVariation::Group30Var2") {
					analogs[i].variation = StaticAnalogVariation::Group30Var2;
				} else if (variation == "StaticAnalogVariation::Group30Var3") {
					analogs[i].variation = StaticAnalogVariation::Group30Var3;
				} else if (variation == "StaticAnalogVariation::Group30Var4") {
					analogs[i].variation = StaticAnalogVariation::Group30Var4;
				} else if (variation == "StaticAnalogVariation::Group30Var5") {
					analogs[i].variation = StaticAnalogVariation::Group30Var5;
				} else if (variation == "StaticAnalogVariation::Group30Var6") {
					analogs[i].variation = StaticAnalogVariation::Group30Var6;
				}
			}
			if (hasMetadata(outstationNode, type, i)) {
				if (hasMetadataClazz(outstationNode, type, i)) {
					analogs[i].metadata.clazz = getPointClass(getMetadataClazz(outstationNode, type, i));
				}
				if (hasMetadataVariation(outstationNode, type, i)) {
					std::string metadataVariation = getMetadataVariation(outstationNode, type, i);
					if (metadataVariation == "EventAnalogVariation::Group32Var1") {
						analogs[i].metadata.variation = EventAnalogVariation::Group32Var1;
					} else if (metadataVariation == "EventAnalogVariation::Group32Var2") {
						analogs[i].metadata.variation = EventAnalogVariation::Group32Var2;
					} else if (metadataVariation == "EventAnalogVariation::Group32Var3") {
						analogs[i].metadata.variation = EventAnalogVariation::Group32Var3;
					} else if (metadataVariation == "EventAnalogVariation::Group32Var4") {
						analogs[i].metadata.variation = EventAnalogVariation::Group32Var4;
					} else if (metadataVariation == "EventAnalogVariation::Group32Var5") {
						analogs[i].metadata.variation = EventAnalogVariation::Group32Var5;
					} else if (metadataVariation == "EventAnalogVariation::Group32Var6") {
						analogs[i].metadata.variation = EventAnalogVariation::Group32Var6;
					} else if (metadataVariation == "EventAnalogVariation::Group32Var7") {
						analogs[i].metadata.variation = EventAnalogVariation::Group32Var7;
					} else if (metadataVariation == "EventAnalogVariation::Group32Var8") {
						analogs[i].metadata.variation = EventAnalogVariation::Group32Var8;
					} else {
						std::cerr << "OutstationAppConfig: unable to map metadataVariation[" << metadataVariation << "]" << std::endl;
					}
				}
			}
		}
	}
	void configureCounters(YAML::Node& outstationNode, openpal::ArrayView<Cell<Counter>, uint16_t>& counters) {
		const char* type = "counters";
		for (int i = 0; i < counters.Size(); i++) {
			if (hasVariation(outstationNode, type, i)) {
				std::string variation = getVariation(outstationNode, type, i);
				if (variation == "StaticCounterVariation::Group20Var1") {
					counters[i].variation = StaticCounterVariation::Group20Var1;
				} else if (variation == "StaticCounterVariation::Group20Var2") {
					counters[i].variation = StaticCounterVariation::Group20Var2;
				} else if (variation == "StaticCounterVariation::Group20Var5") {
					counters[i].variation = StaticCounterVariation::Group20Var5;
				} else if (variation == "StaticCounterVariation::Group20Var6") {
					counters[i].variation = StaticCounterVariation::Group20Var6;
				}
			}
			if (hasMetadata(outstationNode, type, i)) {
				if (hasMetadataClazz(outstationNode, type, i)) {
					counters[i].metadata.clazz = getPointClass(getMetadataClazz(outstationNode, type, i));
				}
				if (hasMetadataVariation(outstationNode, type, i)) {
					std::string metadataVariation = getMetadataVariation(outstationNode, type, i);
					if (metadataVariation == "EventCounterVariation::Group22Var1") {
						counters[i].metadata.variation = EventCounterVariation::Group22Var1;
					} else if (metadataVariation == "EventCounterVariation::Group22Var2") {
						counters[i].metadata.variation = EventCounterVariation::Group22Var2;
					} else if (metadataVariation == "EventCounterVariation::Group22Var5") {
						counters[i].metadata.variation = EventCounterVariation::Group22Var5;
					} else if (metadataVariation == "EventCounterVariation::Group22Var6") {
						counters[i].metadata.variation = EventCounterVariation::Group22Var6;
					} else {
						std::cerr << "OutstationAppConfig: unable to map metadataVariation[" << metadataVariation << "]" << std::endl;
					}
				}
			}
		}
	}
	void configureFrozenCounters(YAML::Node& outstationNode, openpal::ArrayView<Cell<FrozenCounter>, uint16_t>& frozenCounters) {
		const char* type = "frozenCounters";
		for (int i = 0; i < frozenCounters.Size(); i++) {
			if (hasVariation(outstationNode, type, i)) {
				std::string variation = getVariation(outstationNode, type, i);
				if (variation == "StaticFrozenCounterVariation::Group21Var1") {
					frozenCounters[i].variation = StaticFrozenCounterVariation::Group21Var1;
				} else if (variation == "StaticFrozenCounterVariation::Group21Var2") {
					frozenCounters[i].variation = StaticFrozenCounterVariation::Group21Var2;
				} else if (variation == "StaticFrozenCounterVariation::Group21Var5") {
					frozenCounters[i].variation = StaticFrozenCounterVariation::Group21Var5;
				} else if (variation == "StaticFrozenCounterVariation::Group21Var6") {
					frozenCounters[i].variation = StaticFrozenCounterVariation::Group21Var6;
				} else if (variation == "StaticFrozenCounterVariation::Group21Var9") {
					frozenCounters[i].variation = StaticFrozenCounterVariation::Group21Var9;
				} else if (variation == "StaticFrozenCounterVariation::Group21Var10") {
					frozenCounters[i].variation = StaticFrozenCounterVariation::Group21Var10;
				}
			}
			if (hasMetadata(outstationNode, type, i)) {
				if (hasMetadataClazz(outstationNode, type, i)) {
					frozenCounters[i].metadata.clazz = getPointClass(getMetadataClazz(outstationNode, type, i));
				}
				if (hasMetadataVariation(outstationNode, type, i)) {
					std::string metadataVariation = getMetadataVariation(outstationNode, type, i);
					if (metadataVariation == "EventFrozenCounterVariation::Group23Var1") {
						frozenCounters[i].metadata.variation = EventFrozenCounterVariation::Group23Var1;
					} else if (metadataVariation == "EventFrozenCounterVariation::Group23Var2") {
						frozenCounters[i].metadata.variation = EventFrozenCounterVariation::Group23Var2;
					} else if (metadataVariation == "EventFrozenCounterVariation::Group23Var5") {
						frozenCounters[i].metadata.variation = EventFrozenCounterVariation::Group23Var5;
					} else if (metadataVariation == "EventFrozenCounterVariation::Group23Var6") {
						frozenCounters[i].metadata.variation = EventFrozenCounterVariation::Group23Var6;
					} else {
						std::cerr << "OutstationAppConfig: unable to map metadataVariation[" << metadataVariation << "]" << std::endl;
					}
				}
			}
		}
	}
	void configureBinaryOutputStatii(YAML::Node& outstationNode, openpal::ArrayView<Cell<BinaryOutputStatus>, uint16_t>& binaryOutputStatii) {
		const char* type = "binaryOutputStatii";
		for (int i = 0; i < binaryOutputStatii.Size(); i++) {
			if (hasVariation(outstationNode, type, i)) {
				std::string variation = getVariation(outstationNode, type, i);
				if (variation == "StaticBinaryOutputStatusVariation::Group10Var2") {
					binaryOutputStatii[i].variation = StaticBinaryOutputStatusVariation::Group10Var2;
				}
			}
			if (hasMetadata(outstationNode, type, i)) {
				if (hasMetadataClazz(outstationNode, type, i)) {
					binaryOutputStatii[i].metadata.clazz = getPointClass(getMetadataClazz(outstationNode, type, i));
				}
				if (hasMetadataVariation(outstationNode, type, i)) {
					std::string metadataVariation = getMetadataVariation(outstationNode, type, i);
					if (metadataVariation == "EventBinaryOutputStatusVariation::Group11Var1") {
						binaryOutputStatii[i].metadata.variation = EventBinaryOutputStatusVariation::Group11Var1;
					} else if (metadataVariation == "EventBinaryOutputStatusVariation::Group11Var2") {
						binaryOutputStatii[i].metadata.variation = EventBinaryOutputStatusVariation::Group11Var2;
					} else {
						std::cerr << "OutstationAppConfig: unable to map metadataVariation[" << metadataVariation << "]" << std::endl;
					}
				}
			}
		}
	}
	void configureAnalogOutputStatii(YAML::Node& outstationNode, openpal::ArrayView<Cell<AnalogOutputStatus>, uint16_t>& analogOutputStatii) {
		const char* type = "analogOutputStatii";
		for (int i = 0; i < analogOutputStatii.Size(); i++) {
			if (hasVariation(outstationNode, type, i)) {
				std::string variation = getVariation(outstationNode, type, i);
				if (variation == "StaticAnalogOutputStatusVariation::Group40Var1") {
					analogOutputStatii[i].variation = StaticAnalogOutputStatusVariation::Group40Var1;
				} else if (variation == "StaticAnalogOutputStatusVariation::Group40Var2") {
					analogOutputStatii[i].variation = StaticAnalogOutputStatusVariation::Group40Var2;
				} else if (variation == "StaticAnalogOutputStatusVariation::Group40Var3") {
					analogOutputStatii[i].variation = StaticAnalogOutputStatusVariation::Group40Var3;
				} else if (variation == "StaticAnalogOutputStatusVariation::Group40Var4") {
					analogOutputStatii[i].variation = StaticAnalogOutputStatusVariation::Group40Var4;
				}
			}
			if (hasMetadata(outstationNode, type, i)) {
				if (hasMetadataClazz(outstationNode, type, i)) {
					analogOutputStatii[i].metadata.clazz = getPointClass(getMetadataClazz(outstationNode, type, i));
				}
				if (hasMetadataVariation(outstationNode, type, i)) {
					std::string metadataVariation = getMetadataVariation(outstationNode, type, i);
					if (metadataVariation == "EventAnalogOutputStatusVariation::Group42Var1") {
						analogOutputStatii[i].metadata.variation = EventAnalogOutputStatusVariation::Group42Var1;
					} else if (metadataVariation == "EventAnalogOutputStatusVariation::Group42Var2") {
						analogOutputStatii[i].metadata.variation = EventAnalogOutputStatusVariation::Group42Var2;
					} else if (metadataVariation == "EventAnalogOutputStatusVariation::Group42Var3") {
						analogOutputStatii[i].metadata.variation = EventAnalogOutputStatusVariation::Group42Var3;
					} else if (metadataVariation == "EventAnalogOutputStatusVariation::Group42Var4") {
						analogOutputStatii[i].metadata.variation = EventAnalogOutputStatusVariation::Group42Var4;
					} else if (metadataVariation == "EventAnalogOutputStatusVariation::Group42Var5") {
						analogOutputStatii[i].metadata.variation = EventAnalogOutputStatusVariation::Group42Var5;
					} else if (metadataVariation == "EventAnalogOutputStatusVariation::Group42Var6") {
						analogOutputStatii[i].metadata.variation = EventAnalogOutputStatusVariation::Group42Var6;
					} else if (metadataVariation == "EventAnalogOutputStatusVariation::Group42Var7") {
						analogOutputStatii[i].metadata.variation = EventAnalogOutputStatusVariation::Group42Var7;
					} else if (metadataVariation == "EventAnalogOutputStatusVariation::Group42Var8") {
						analogOutputStatii[i].metadata.variation = EventAnalogOutputStatusVariation::Group42Var8;
					} else {
						std::cerr << "OutstationAppConfig: unable to map metadataVariation[" << metadataVariation << "]" << std::endl;
					}
				}
			}
		}
	}
	void configureTimeAndIntervals(YAML::Node& outstationNode, openpal::ArrayView<Cell<TimeAndInterval>, uint16_t>& timeAndIntervals) {
		const char* type = "timeAndIntervals";
		for (int i = 0; i < timeAndIntervals.Size(); i++) {
			if (hasVariation(outstationNode, type, i)) {
				std::string variation = getVariation(outstationNode, type, i);
				if (variation == "StaticTimeAndIntervalVariation::Group50Var4") {
					timeAndIntervals[i].variation = StaticTimeAndIntervalVariation::Group50Var4;
				}
			}
		}
	}

	bool hasVariation(YAML::Node& outstationNode, const char* type, int idx) {
		return outstationNode["measDB"][type][idx]["variation"];
	}
	std::string getVariation(YAML::Node& outstationNode, const char* type, int idx) {
		return outstationNode["measDB"][type][idx]["variation"].as<std::string>();
	}
	bool hasMetadata(YAML::Node& outstationNode, const char* type, int idx) {
		return outstationNode["measDB"][type][idx]["metadata"];
	}
	bool hasMetadataClazz(YAML::Node& outstationNode, const char* type, int idx) {
		return outstationNode["measDB"][type][idx]["metadata"]["clazz"];
	}
	std::string getMetadataClazz(YAML::Node& outstationNode, const char* type, int idx) {
		return outstationNode["measDB"][type][idx]["metadata"]["clazz"].as<std::string>();
	}
	bool hasMetadataVariation(YAML::Node& outstationNode, const char* type, int idx) {
		return outstationNode["measDB"][type][idx]["metadata"]["variation"];
	}
	std::string getMetadataVariation(YAML::Node& outstationNode, const char* type, int idx) {
		return outstationNode["measDB"][type][idx]["metadata"]["variation"].as<std::string>();
	}
	PointClass getPointClass(std::string metadataClazz) {
		if (metadataClazz == "PointClass::Class0") {
			return PointClass::Class0;
		} else if (metadataClazz == "PointClass::Class1") {
			return PointClass::Class1;
		} else if (metadataClazz == "PointClass::Class2") {
			return PointClass::Class2;
		} else if (metadataClazz == "PointClass::Class3") {
			return PointClass::Class3;
		} else {
			std::cerr << "OutstationAppConfig: unable to map metadataClazz[" << metadataClazz << "] defaulting to PointClass::Class1" << std::endl;
			return PointClass::Class1;
		}
	}

	YAML::Node configNode_;
}
;
