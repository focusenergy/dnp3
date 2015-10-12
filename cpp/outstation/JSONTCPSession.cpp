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

#include <asiodnp3/MeasUpdate.h>
#include <boost/asio.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <stdlib.h>

using boost::asio::ip::tcp;

using namespace asiodnp3;
using namespace rapidjson;

/**
 * JSONTCPSession reads/writes JSON objects to socket that represent Outstation commands.
 * Protocol is as follows: [int64_t size][size bytes of JSON]
 */
class JSONTCPSession: public std::enable_shared_from_this<JSONTCPSession> {
public:
	JSONTCPSession(tcp::socket socket, std::map<std::string, IOutstation*>& outstations) :
			socket_(std::move(socket)), outstations_ { outstations } {
	}

	void start() {
		read();
	}

	void write(AsyncCommand* command) {
		StringBuffer out_json_sb = toJSONStringBuffer(command);

		// Get JSON string length, convert size to int64_t and write in character stream
		int64_t json_size = out_json_sb.GetSize();

		// Concat raw json_size bytes and JSON bytes
		char out_data[sizeof(int64_t) + out_json_sb.GetSize()];
		memcpy(out_data, &json_size, sizeof(int64_t));
		memcpy(out_data + sizeof(int64_t), out_json_sb.GetString(), out_json_sb.GetSize());

		// Write data to socket
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(out_data, sizeof(out_data)), [this, self](boost::system::error_code ec, std::size_t length) {
			// TODO handle errors
			});
	}

	bool is_active() {
		return socket_.is_open();
	}

private:
	// Type constants (AsyncCommand)
	static constexpr const char* AI16 = "AnalogInt16";
	static constexpr const char* AI32 = "AnalogInt32";
	static constexpr const char* AF32 = "AnalogFloat32";
	static constexpr const char* AD64 = "AnalogDouble64";
	static constexpr const char* CROB = "ControlRelayOutputBlock";

	static constexpr const char* BIN = "Binary";
	static constexpr const char* DBBIN = "DoubleBitBinary";
	static constexpr const char* CI32 = "CounterInt32";
	static constexpr const char* FCI32 = "FrozenCounterInt32";

	/**
	 * Reads incoming JSON preceded by int64_t with number of bytes to read.
	 */
	void read() {
		// TODO add buffer size vs data size validation and error handling
		auto self(shared_from_this());
		socket_.async_receive(boost::asio::buffer(buf_, buf_sz), [this, self](boost::system::error_code error, size_t length)
		{
			if (!error)
			{
				size_t buf_pos = 0;
				while (buf_pos < length) {
					size_t buf_rem = length - buf_pos;
					if (data_pos_ == 0) {
						/** initialize data size*/
						memcpy(&data_sz_, &buf_[buf_pos], sizeof(int64_t));
						if (data_sz_ < 0) {
							/** TODO warn in log **/
							data_sz_ = 0;
						}
						buf_pos += sizeof(int64_t);
						buf_rem -= sizeof(int64_t);

						/** initialize data array */
						data_ = new char[data_sz_+1];
						data_[data_sz_] = '\0';

						/** copy data from buffer (max(data_sz_,length)) */
						size_t size = buf_rem > (size_t) data_sz_ ? data_sz_: buf_rem;
						memcpy(data_, &buf_[buf_pos], size);
						data_pos_ = size;
						buf_pos += size;
					} else if(data_pos_ < (size_t) data_sz_) {
						/** copy data from buffer (max(data_rem,buf_rem)) */
						size_t data_rem = data_sz_ - data_pos_;
						size_t size = buf_rem > data_rem ? data_rem : buf_rem;
						memcpy(data_ + data_pos_, &buf_[buf_pos], size);
						buf_pos += size;
					}
					/** if data_sz has been transferred, process JSON object */
					if (data_pos_ == (size_t) data_sz_) {
						/** apply update from JSON chars, deallocate memory and reset pointers */
						applyUpdate(data_);
						delete[] data_;
						data_pos_ = 0;
						data_sz_ = 0;
					}
				}
				read();
			} else if (error.value() == boost::system::errc::no_such_file_or_directory) {
				/** read() returned EOF, shutdown entire socket **/
				socket_.shutdown(socket_.shutdown_both);
				socket_.close();
			} else {
				std::cerr << "Error occurred in TCP session " << error << std::endl;
			}
		});
	}

	/**
	 * Takes input AsyncCommand and serializes JSON object to resulting StringBuffer
	 */
	StringBuffer toJSONStringBuffer(AsyncCommand* command) {
		StringBuffer sb;
		Writer<StringBuffer> writer(sb);
		writer.StartObject();
		/** TODO add outstation id */
		writer.String("index");
		writer.Int(command->Idx());
		if (command->AOInt16() != NULL) {
			writer.String("type");
			writer.String(AI16);
			writer.String("value");
			writer.Int(command->AOInt16()->value);
			writer.String("status");
			writer.Int(static_cast<uint16_t>(command->AOInt16()->status));
		} else if (command->AOInt32() != NULL) {
			writer.String("type");
			writer.String(AI32);
			writer.String("value");
			writer.Int(command->AOInt32()->value);
			writer.String("status");
			writer.Int(static_cast<uint16_t>(command->AOInt32()->status));
		} else if (command->AOFloat32() != NULL) {
			writer.String("type");
			writer.String(AF32);
			writer.String("value");
			writer.Double(command->AOFloat32()->value);
			writer.String("status");
			writer.Int(static_cast<uint16_t>(command->AOFloat32()->status));
		} else if (command->AODouble64() != NULL) {
			writer.String("type");
			writer.String(AD64);
			writer.String("value");
			writer.Double(command->AODouble64()->value);
			writer.String("status");
			writer.Int(static_cast<uint16_t>(command->AODouble64()->status));
		} else if (command->CROB() != NULL) {
			// TODO handle all CROB attributes
			writer.String("type");
			writer.String(CROB);
			writer.String("value");
			writer.Int(static_cast<uint16_t>(command->CROB()->functionCode));
			writer.String("status");
			writer.Int(static_cast<uint16_t>(command->CROB()->status));
		} else {
			std::cerr << "JSONTCPSession: command not recognized" << std::endl;
		}
		writer.EndObject();
		return sb;
	}

	/**
	 * Takes a char[] JSON object and de-serializes to a new MeasUpdate which is applied against pOustation_
	 */
	void applyUpdate(char* pchar_json_data) {
		Document d;
		d.ParseInsitu(pchar_json_data);
		if (d.IsObject()) {
			if (d.HasMember("id") && d["id"].IsString() && d.HasMember("type") && d["type"].IsString() && d.HasMember("index") && d["index"].IsInt()
					&& d.HasMember("value")) {
				if (outstations_.count(d["id"].GetString())) {
					IOutstation* pOutstation = outstations_[d["id"].GetString()];
					MeasUpdate update(pOutstation);
					const char* type = d["type"].GetString();
					if (!strcmp(AD64, type)) {
						if (d["value"].IsDouble()) {
							update.Update(Analog(d["value"].GetDouble()), d["index"].GetInt());
						}
					} else if (!strcmp(BIN, type)) {
						if (d["value"].IsBool()) {
							update.Update(Binary(d["value"].GetBool()), d["index"].GetInt());
						}
					} else if (!strcmp(CI32, type)) {
						if (d["value"].IsInt()) {
							update.Update(Counter(d["value"].GetInt()), d["index"].GetInt());
						}
					} else if (!strcmp(FCI32, type)) {
						if (d["value"].IsInt()) {
							update.Update(FrozenCounter(d["value"].GetInt()), d["index"].GetInt());
						}
					} else {
						std::cerr << "JSONTCPSession: type attribute[" << type << "]not recognized" << std::endl;
					}
				} else {
					std::cerr << "JSONTCPSession: object id not found in outstations map" << std::endl;
				}
			} else {
				std::cerr << "JSONTCPSession: object missing id or type attribute" << std::endl;
			}
		} else {
			std::cerr << "JSONTCPSession: not an object" << std::endl;
		}
	}

	enum {
		buf_sz = 1024

	};
	char buf_[buf_sz];

	char* data_ = NULL;
	size_t data_pos_ = 0;
	int64_t data_sz_ = 0;

	tcp::socket socket_;
	std::map<std::string, IOutstation*> outstations_;
}
;
