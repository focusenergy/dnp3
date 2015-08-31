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

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>

#include "JSONCommandHandler.h"

using namespace opendnp3;
using namespace rapidjson;

// TODO do more than just print out JSON!

CommandStatus JSONCommandHandler::Select(const ControlRelayOutputBlock& command, uint16_t aIndex) {
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("select");
	writer.StartObject();
	writer.String("command");
	writer.String("ControlRelayOutputBlock");
	writer.String("index");
	writer.Int(aIndex);
	writer.String("functionCode");
	writer.Int(static_cast<int>(command.functionCode));
	writer.String("status");
	writer.Int(static_cast<int>(command.status));
	writer.EndObject();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
	return CommandStatus::SUCCESS;
}
CommandStatus JSONCommandHandler::Operate(const ControlRelayOutputBlock& command, uint16_t aIndex) {
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("operate");
	writer.StartObject();
	writer.String("command");
	writer.String("ControlRelayOutputBlock");
	writer.String("index");
	writer.Int(aIndex);
	writer.String("functionCode");
	writer.Int(static_cast<int>(command.functionCode));
	writer.String("status");
	writer.Int(static_cast<int>(command.status));
	writer.EndObject();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
	return CommandStatus::SUCCESS;
}

CommandStatus JSONCommandHandler::Select(const AnalogOutputInt16& command, uint16_t aIndex) {
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("select");
	writer.StartObject();
	writer.String("command");
	writer.String("AnalogOutputInt16");
	writer.String("index");
	writer.Int(aIndex);
	writer.String("value");
	writer.Int(command.value);
	writer.String("status");
	writer.Int(static_cast<int>(command.status));
	writer.EndObject();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
	return CommandStatus::SUCCESS;
}
CommandStatus JSONCommandHandler::Operate(const AnalogOutputInt16& command, uint16_t aIndex) {
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("operate");
	writer.StartObject();
	writer.String("command");
	writer.String("AnalogOutputInt16");
	writer.String("index");
	writer.Int(aIndex);
	writer.String("value");
	writer.Int(command.value);
	writer.String("status");
	writer.Int(static_cast<int>(command.status));
	writer.EndObject();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
	return CommandStatus::SUCCESS;
}

CommandStatus JSONCommandHandler::Select(const AnalogOutputInt32& command, uint16_t aIndex) {
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("select");
	writer.StartObject();
	writer.String("command");
	writer.String("AnalogOutputInt32");
	writer.String("index");
	writer.Int(aIndex);
	writer.String("value");
	writer.Int(command.value);
	writer.String("status");
	writer.Int(static_cast<int>(command.status));
	writer.EndObject();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
	return CommandStatus::SUCCESS;
}
CommandStatus JSONCommandHandler::Operate(const AnalogOutputInt32& command, uint16_t aIndex) {
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("operate");
	writer.StartObject();
	writer.String("command");
	writer.String("AnalogOutputInt32");
	writer.String("index");
	writer.Int(aIndex);
	writer.String("value");
	writer.Int(command.value);
	writer.String("status");
	writer.Int(static_cast<int>(command.status));
	writer.EndObject();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
	return CommandStatus::SUCCESS;
}

CommandStatus JSONCommandHandler::Select(const AnalogOutputFloat32& command, uint16_t aIndex) {
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("select");
	writer.StartObject();
	writer.String("command");
	writer.String("AnalogOutputFloat32");
	writer.String("index");
	writer.Int(aIndex);
	writer.String("value");
	writer.Double(command.value);
	writer.String("status");
	writer.Int(static_cast<int>(command.status));
	writer.EndObject();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
	return CommandStatus::SUCCESS;
}
CommandStatus JSONCommandHandler::Operate(const AnalogOutputFloat32& command, uint16_t aIndex) {
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("select");
	writer.StartObject();
	writer.String("command");
	writer.String("AnalogOutputFloat32");
	writer.String("index");
	writer.Int(aIndex);
	writer.String("value");
	writer.Double(command.value);
	writer.String("status");
	writer.Int(static_cast<int>(command.status));
	writer.EndObject();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
	return CommandStatus::SUCCESS;
}

CommandStatus JSONCommandHandler::Select(const AnalogOutputDouble64& command, uint16_t aIndex) {
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("select");
	writer.StartObject();
	writer.String("command");
	writer.String("AnalogOutputDouble64");
	writer.String("index");
	writer.Int(aIndex);
	writer.String("value");
	writer.Double(command.value);
	writer.String("status");
	writer.Int(static_cast<int>(command.status));
	writer.EndObject();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
	return CommandStatus::SUCCESS;
}
CommandStatus JSONCommandHandler::Operate(const AnalogOutputDouble64& command, uint16_t aIndex) {
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("operate");
	writer.StartObject();
	writer.String("command");
	writer.String("AnalogOutputDouble64");
	writer.String("index");
	writer.Int(aIndex);
	writer.String("value");
	writer.Double(command.value);
	writer.String("status");
	writer.Int(static_cast<int>(command.status));
	writer.EndObject();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
	return CommandStatus::SUCCESS;
}
