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

#include "AsyncCommandHandler.h"

// TODO validate requests before returning CommandStatus
AsyncCommandHandler::AsyncCommandHandler(const char* id, AsyncCommandQueue& queue) :
		id_(id), queue_(queue) {
}

CommandStatus AsyncCommandHandler::Select(const ControlRelayOutputBlock& command, uint16_t aIndex) {
	return CommandStatus::SUCCESS;
}
CommandStatus AsyncCommandHandler::Operate(const ControlRelayOutputBlock& command, uint16_t aIndex) {
	queue_.push(new AsyncCommand(&command, id_, aIndex));
	return CommandStatus::SUCCESS;
}

CommandStatus AsyncCommandHandler::Select(const AnalogOutputInt16& command, uint16_t aIndex) {
	return CommandStatus::SUCCESS;
}
CommandStatus AsyncCommandHandler::Operate(const AnalogOutputInt16& command, uint16_t aIndex) {
	queue_.push(new AsyncCommand(&command, id_, aIndex));
	return CommandStatus::SUCCESS;
}

CommandStatus AsyncCommandHandler::Select(const AnalogOutputInt32& command, uint16_t aIndex) {
	return CommandStatus::SUCCESS;
}
CommandStatus AsyncCommandHandler::Operate(const AnalogOutputInt32& command, uint16_t aIndex) {
	queue_.push(new AsyncCommand(&command, id_, aIndex));
	return CommandStatus::SUCCESS;
}

CommandStatus AsyncCommandHandler::Select(const AnalogOutputFloat32& command, uint16_t aIndex) {
	return CommandStatus::SUCCESS;
}
CommandStatus AsyncCommandHandler::Operate(const AnalogOutputFloat32& command, uint16_t aIndex) {
	queue_.push(new AsyncCommand(&command, id_, aIndex));
	return CommandStatus::SUCCESS;
}

CommandStatus AsyncCommandHandler::Select(const AnalogOutputDouble64& command, uint16_t aIndex) {
	return CommandStatus::SUCCESS;
}
CommandStatus AsyncCommandHandler::Operate(const AnalogOutputDouble64& command, uint16_t aIndex) {
	queue_.push(new AsyncCommand(&command, id_, aIndex));
	return CommandStatus::SUCCESS;
}

void AsyncCommandHandler::Start() {
	// Do nothing
}

void AsyncCommandHandler::End() {
	// Do nothing
}

