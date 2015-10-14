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

using namespace opendnp3;

class AsyncCommand {
public:
	AsyncCommand(const ControlRelayOutputBlock* crob, const char* id, uint16_t idx) :
			id_(id), idx_(idx), crob_(crob), aoInt16_(), aoInt32_(), aoFloat32_(), aoDouble64_() {
	}
	AsyncCommand(const AnalogOutputInt16* aoInt16, const char* id, uint16_t idx) :
			id_(id), idx_(idx), crob_(), aoInt16_(aoInt16), aoInt32_(), aoFloat32_(), aoDouble64_() {
	}
	AsyncCommand(const AnalogOutputInt32* aoInt32, const char* id, uint16_t idx) :
			id_(id), idx_(idx), crob_(), aoInt16_(), aoInt32_(aoInt32), aoFloat32_(), aoDouble64_() {
	}
	AsyncCommand(const AnalogOutputFloat32* aoFloat32, const char* id, uint16_t idx) :
			id_(id), idx_(idx), crob_(), aoInt16_(), aoInt32_(), aoFloat32_(aoFloat32), aoDouble64_() {
	}
	AsyncCommand(const AnalogOutputDouble64* aoDouble64, const char* id, uint16_t idx) :
			id_(id), idx_(idx), crob_(), aoInt16_(), aoInt32_(), aoFloat32_(), aoDouble64_(aoDouble64) {
	}

	const char* Id() {
		return id_;
	}

	uint16_t Idx() {
		return idx_;
	}
	const ControlRelayOutputBlock* CROB() {
		return crob_;
	}
	const AnalogOutputInt16* AOInt16() {
		return aoInt16_;
	}
	const AnalogOutputInt32* AOInt32() {
		return aoInt32_;
	}
	const AnalogOutputFloat32* AOFloat32() {
		return aoFloat32_;
	}
	const AnalogOutputDouble64* AODouble64() {
		return aoDouble64_;
	}

private:
	const char* id_;
	uint16_t idx_;
	const ControlRelayOutputBlock* crob_;
	const AnalogOutputInt16* aoInt16_;
	const AnalogOutputInt32* aoInt32_;
	const AnalogOutputFloat32* aoFloat32_;
	const AnalogOutputDouble64* aoDouble64_;
};
