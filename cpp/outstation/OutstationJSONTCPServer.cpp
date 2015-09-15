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
#include <thread>

#include "JSONTCPSession.cpp"

using namespace asiodnp3;

class OutstationJSONTCPServer {
public:
	OutstationJSONTCPServer(boost::asio::io_service& io_service, short port, IOutstation* pOutstation, AsyncCommandHandler& handler) :
			handler_(handler), pOutstation_ { pOutstation }, acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), socket_(io_service) {
		start();
	}

private:
	void start() {
		// TODO shutdown thread correctly on SIGINT
		std::thread t([this] {subscribe();});
		t.detach();
		accept();
	}

	void subscribe() {
		while (true) {
			AsyncCommand command = handler_.pop();
			std::set<std::shared_ptr<JSONTCPSession>>::iterator it;
			for (it = sessions_.begin(); it != sessions_.end();) {
				if (it->get()->is_active()) {
					it->get()->write(command);
					it++;
				} else {
					sessions_.erase(it++);
				}
			}
		}
	}

	void accept() {
		acceptor_.async_accept(socket_, [this](boost::system::error_code error)
		{
			if (!error)
			{
				std::shared_ptr<JSONTCPSession> pSession = std::make_shared<JSONTCPSession>(std::move(socket_), pOutstation_);
				sessions_.insert(pSession);
				pSession->start();
			} else {
				std::cerr << "Unable to create TCP Session " << error << std::endl;
			}
			accept();
		});
	}

	IOutstation* pOutstation_;
	AsyncCommandHandler& handler_;
	std::set<std::shared_ptr<JSONTCPSession>> sessions_;

	tcp::acceptor acceptor_;
	tcp::socket socket_;
};
