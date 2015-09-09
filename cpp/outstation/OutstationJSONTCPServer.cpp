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

#include <set>
#include "JSONTCPSession.cpp"

using namespace asiodnp3;

class OutstationJSONTCPServer {
public:
	OutstationJSONTCPServer(boost::asio::io_service& io_service, short port, IOutstation* pOutstation, AsyncCommandHandler& handler) :
			handler_(handler), pOutstation_ { pOutstation }, acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), socket_(io_service) {
		do_accept();
	}

private:
	void do_accept() {
		acceptor_.async_accept(socket_, [this](boost::system::error_code error)
		{
			if (!error)
			{
				std::make_shared<JSONTCPSession>(std::move(socket_), pOutstation_)->start();
			} else {
				std::cerr << "Unable to create TCP Session " << error << std::endl;
			}
			do_accept();
		});
	}

	IOutstation* pOutstation_;
	AsyncCommandHandler& handler_;
	tcp::acceptor acceptor_;
	tcp::socket socket_;
};
