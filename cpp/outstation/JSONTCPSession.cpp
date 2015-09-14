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
 * JSONTCPSession reads/writes JSON objects to socket that represent outstation commands.
 * Protocol is as follows: [int64_t size][size bytes of JSON]
 */
class JSONTCPSession: public std::enable_shared_from_this<JSONTCPSession> {
public:
	JSONTCPSession(tcp::socket socket, IOutstation* pOutstation) :
			socket_(std::move(socket)), pOutstation_ { pOutstation } {
	}

	void start() {
		read();
	}

	void write(AsyncCommand command) {
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

private:
	/**
	 * Reads incoming JSON preceded by int64_t with number of bytes to read.
	 */
	void read() {
		auto self(shared_from_this());
		socket_.async_receive(boost::asio::buffer(buf_, buf_sz_), [this, self](boost::system::error_code error, size_t length)
		{
			if (!error)
			{
				// Initialize for new JSON object in stream
				if (data_pos_ == 0) {
					// Initialize data_sz_
					memcpy(&data_sz_, &buf_, sizeof(int64_t));

					// Initialize data, copy first buffer's worth of chars
					data_ = new char[data_sz_];
					data_pos_ = length - sizeof(int64_t);
					memcpy(data_, &buf_[sizeof(int64_t)], data_pos_);
				} else if(data_pos_ < data_sz_) {
					// copy next buffer's worth of chars
					memcpy(data_ + data_pos_, buf_, length);
					data_pos_ += length;
				}
				// if all chars have been transferred, process JSON object
				if (data_pos_ == data_sz_) {
					// apply update from JSON chars, deallocate memory and reset pointers
					applyUpdate(data_);
					delete[] data_;
					data_pos_ = 0;
					data_sz_ = 0;
				}
				read();
			} else if (error.value() == boost::system::errc::no_such_file_or_directory) {
				// Socket is closed
			} else {
				std::cerr << "Error occurred in TCP session " << error << std::endl;
			}
		});
	}

	/**
	 * Takes input AsyncCommand and serializes JSON object to resulting StringBuffer
	 */
	StringBuffer toJSONStringBuffer(AsyncCommand command) {
		StringBuffer sb;
		Writer<StringBuffer> writer(sb);
		writer.StartObject();
		writer.String("index");
		writer.Int(command.Idx());
		if (command.AOInt16() != NULL) {
			writer.String("type");
			writer.String("AnalogInt16");
			writer.String("value");
			writer.Int(command.AOInt16()->value);
			writer.String("status");
			writer.Int(static_cast<uint16_t>(command.AOInt16()->status));
		} else if (command.AOInt32() != NULL) {
			writer.String("type");
			writer.String("AnalogInt32");
			writer.String("value");
			writer.Int(command.AOInt32()->value);
			writer.String("status");
			writer.Int(static_cast<uint16_t>(command.AOInt32()->status));
		} else if (command.AOFloat32() != NULL) {
			writer.String("type");
			writer.String("AnalogFloat32");
			writer.String("value");
			writer.Double(command.AOFloat32()->value);
			writer.String("status");
			writer.Int(static_cast<uint16_t>(command.AOFloat32()->status));
		} else if (command.AODouble64() != NULL) {
			writer.String("type");
			writer.String("AnalogDouble64");
			writer.String("value");
			writer.Double(command.AODouble64()->value);
			writer.String("status");
			writer.Int(static_cast<uint16_t>(command.AODouble64()->status));
		} else if (command.CROB() != NULL) {
			// TODO handle all CROB attributes
			writer.String("type");
			writer.String("ControlRelayOutputBlock");
			writer.String("value");
			writer.Int(static_cast<uint16_t>(command.CROB()->functionCode));
			writer.String("status");
			writer.Int(static_cast<uint16_t>(command.CROB()->status));
		} else {
			//TODO exception handle!
		}
		writer.EndObject();
		return sb;
	}

	/**
	 * Takes a char[] JSON object and de-serializes to a new MeasUpdate which is applied against pOustation_
	 */
	void applyUpdate(char* pchar_json_data) {
		std::cout << "applying update: " << pchar_json_data << std::endl;
		Document d;
		d.ParseInsitu(pchar_json_data);
		if (d.IsObject()) {
			MeasUpdate update(pOutstation_);
			if (d.HasMember("type") && d["type"].IsString() && d.HasMember("index") && d["index"].IsInt()) {
				std::cout << d["type"].GetString() << std::endl;
				/* TODO FIX ME
				 * char* type = d["type"].GetString();
				if (type == "AnalogInt16") {
					if (d.HasMember("value") && d["value"].IsInt()) {
					update.Update(Analog(d["value"].GetInt()), d["index"].GetInt());
				}}*/
		} else {
			std::cout << "no type" << std::endl;
		}
	} else {
		std::cerr << "not an object!" << std::endl;
	}
}

enum {
	buf_sz_ = 1024
};
char buf_[buf_sz_];

char* data_ = NULL;
size_t data_pos_ = 0;
int64_t data_sz_ = 0;

tcp::socket socket_;
IOutstation* pOutstation_;
}
;
