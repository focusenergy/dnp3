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
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

using boost::asio::ip::tcp;

class TCPSession: public std::enable_shared_from_this<TCPSession> {
public:
	TCPSession(tcp::socket socket) :
			socket_(std::move(socket)) {
	}

	void start() {
		do_read();
	}

private:
	void do_read() {
		auto self(shared_from_this());
		socket_.async_receive(boost::asio::buffer(data_, 1024), [this, self](boost::system::error_code error, std::size_t length)
		{
			if (!error)
			{
				sbuf.sputn(data_,length);
				do_read();
			} else if (error.value() == boost::system::errc::no_such_file_or_directory) {
				std::cout << sbuf.str() << std::endl;
				  void *array[10];
				  size_t size;

				  // get void*'s for all entries on the stack
				  size = backtrace(array, 10);

				  // print out all the frames to stderr
				  fprintf(stderr, "Error:\n");
				  backtrace_symbols_fd(array, size, STDERR_FILENO);
			}
		});
	}

	void do_write(std::size_t length) {
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
		buffer_size = 10
	};
	char data_[buffer_size];
	std::stringbuf sbuf;
}
;
