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
#include <boost/thread/thread.hpp>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

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
		do_read();
	}

private:
	void do_read() {
		auto self(shared_from_this());
		std::cout << "nope!" << this << std::endl;
		std::cout << "yep8" << &socket_ << std::endl;
		socket_.async_receive(boost::asio::buffer(data_, 1024), [this, self](boost::system::error_code error, std::size_t length)
		{
			if (!error)
			{
				sbuf.sputn(data_,length);
				do_read();
			} else if (error.value() == boost::system::errc::no_such_file_or_directory) {
				// Convert stringbuf to char[] and reset
				sbuf.pubseekpos(0);
				char chars[sbuf.in_avail()];
				sbuf.sgetn(chars, sbuf.in_avail());
				sbuf.str(std::string());

				Document d;
				d.ParseInsitu(chars);

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

	void do_write() {
		// TODO serialize JSONCommand to this socket
		/*auto self(shared_from_this());
		 boost::asio::async_write(socket_, boost::asio::buffer(data_, length), [this, self](boost::system::error_code ec, std::size_t length)
		 {
		 if (!ec)
		 {
		 do_read();
		 }
		 });*/
	}

	tcp::socket socket_;
	enum {
		buffer_size = 1024
	};
	char data_[buffer_size];
	std::stringbuf sbuf;
	IOutstation* pOutstation_;
}
;
