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

#include <boost/asio.hpp>
#include <stdlib.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using boost::asio::ip::tcp;

using namespace asiodnp3;
using namespace rapidjson;

class JSONTCPSession: public std::enable_shared_from_this<JSONTCPSession> {
public:
	JSONTCPSession(tcp::socket socket, IOutstation* pOutstation) :
			socket_(std::move(socket)), pOutstation_ { pOutstation } {
	}

	void start() {
		read();
	}

	void write(AsyncCommand command) {
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
		std::cout << sb.GetString() << std::endl;

		//TODO write to socket

		/*auto self(shared_from_this());
		 boost::asio::async_write(socket_, boost::asio::buffer(sb., length), [this, self](boost::system::error_code ec, std::size_t length)
		 {
		 });*/
	}

private:
	void read() {
		auto self(shared_from_this());
		socket_.async_receive(boost::asio::buffer(data_, buffer_size), [this, self](boost::system::error_code error, std::size_t length)
		{
			if (!error)
			{
				data_sbuf_.sputn(data_,length);
				read();
			} else if (error.value() == boost::system::errc::no_such_file_or_directory) {
				// Convert stringbuf to char[] and reset
				data_sbuf_.pubseekpos(0);
				char chars[data_sbuf_.in_avail()];
				data_sbuf_.sgetn(chars, data_sbuf_.in_avail());
				data_sbuf_.str(std::string());

				Document d;
				d.ParseInsitu(chars);

				// TODO create MeasUpdate and apply to pOutstation_, threading issues?
				// Check if JSON object is valid and continue with logic
				if (d.IsObject()) {
					// TODO find specific values and act on them
					if (d.HasMember("stars") && d["stars"].IsInt()) {
						std::cout << d["stars"].GetInt() << std::endl;
					} else {
						std::cout << "nostars" << std::endl;
					}
				} else {
					std::cerr << "not an object!" << std::endl;
				}
			} else {
				std::cerr << "Error occurred in TCP session " << error << std::endl;
			}

		});
	}

	enum {
		buffer_size = 1024
	};
	char data_[buffer_size];
	std::stringbuf data_sbuf_;
	tcp::socket socket_;
	IOutstation* pOutstation_;
}
;
