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
	/*	AsyncCommand() :
	 idx_(0), crob_(NULL), aoInt16_(NULL), aoInt32_(NULL), aoFloat32_(NULL), aoDouble64_(NULL) {
	 }*/
	AsyncCommand(ControlRelayOutputBlock crob, uint16_t idx) :
			idx_(idx), crob_(&crob), aoInt16_(NULL), aoInt32_(NULL), aoFloat32_(NULL), aoDouble64_(NULL) {
	}
	AsyncCommand(AnalogOutputInt16 aoInt16, uint16_t idx) :
			idx_(idx), crob_(NULL), aoInt16_(&aoInt16), aoInt32_(NULL), aoFloat32_(NULL), aoDouble64_(NULL) {
	}
	AsyncCommand(AnalogOutputInt32 aoInt32, uint16_t idx) :
			idx_(idx), crob_(NULL), aoInt16_(NULL), aoInt32_(&aoInt32), aoFloat32_(NULL), aoDouble64_(NULL) {
	}
	AsyncCommand(AnalogOutputFloat32 aoFloat32, uint16_t idx) :
			idx_(idx), crob_(NULL), aoInt16_(NULL), aoInt32_(NULL), aoFloat32_(&aoFloat32), aoDouble64_(NULL) {
	}
	AsyncCommand(AnalogOutputDouble64 aoDouble64, uint16_t idx) :
			idx_(idx), crob_(NULL), aoInt16_(NULL), aoInt32_(NULL), aoFloat32_(NULL), aoDouble64_(&aoDouble64) {
	}

	uint16_t Idx() {
		return idx_;
	}
	ControlRelayOutputBlock* CROB() {
		return crob_;
	}
	AnalogOutputInt16* AOInt16() {
		return aoInt16_;
	}
	AnalogOutputInt32* AOInt32() {
		return aoInt32_;
	}
	AnalogOutputFloat32* AOFloat32() {
		return aoFloat32_;
	}
	AnalogOutputDouble64* AODouble64() {
		return aoDouble64_;
	}

private:
	uint16_t idx_;
	ControlRelayOutputBlock* crob_;
	AnalogOutputInt16* aoInt16_;
	AnalogOutputInt32* aoInt32_;
	AnalogOutputFloat32* aoFloat32_;
	AnalogOutputDouble64* aoDouble64_;
};
