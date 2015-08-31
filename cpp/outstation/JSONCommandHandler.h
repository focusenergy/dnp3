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
#ifndef OPENDNP3_JSONCOMMANDHANDLER_H
#define OPENDNP3_JSONCOMMANDHANDLER_H

#include <opendnp3/outstation/ICommandHandler.h>

namespace opendnp3 {

class JSONCommandHandler: public ICommandHandler {
public:

	CommandStatus Select(const ControlRelayOutputBlock& command, uint16_t aIndex) override final;
	CommandStatus Operate(const ControlRelayOutputBlock& command, uint16_t aIndex) override final;

	CommandStatus Select(const AnalogOutputInt16& command, uint16_t aIndex) override final;
	CommandStatus Operate(const AnalogOutputInt16& command, uint16_t aIndex) override final;

	CommandStatus Select(const AnalogOutputInt32& command, uint16_t aIndex) override final;
	CommandStatus Operate(const AnalogOutputInt32& command, uint16_t aIndex) override final;

	CommandStatus Select(const AnalogOutputFloat32& command, uint16_t aIndex) override final;
	CommandStatus Operate(const AnalogOutputFloat32& command, uint16_t aIndex) override final;

	CommandStatus Select(const AnalogOutputDouble64& command, uint16_t aIndex) override final;
	CommandStatus Operate(const AnalogOutputDouble64& command, uint16_t aIndex) override final;

	static JSONCommandHandler& Instance() {
		static JSONCommandHandler instance;
		return instance;
	}

private:
	JSONCommandHandler() {
	}

	JSONCommandHandler(JSONCommandHandler const&) = delete;
	void operator=(JSONCommandHandler const&) = delete;
};

}

#endif

