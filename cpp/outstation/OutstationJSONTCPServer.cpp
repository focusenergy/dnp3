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

/**
 * OutstationJSONTCPServer handles TCP client sessions over which JSON is transferred.
 *
 * All client sessions are tracked for broadcast messages from the AsyncCommandHandler (i.e. Master server)
 * Clients may send JSON commands which are converted to MeasUpdate by JSONTCPSession.
 *
 */
class OutstationJSONTCPServer {
public:
	OutstationJSONTCPServer(boost::asio::io_service& io_service, short port, IOutstation* pOutstation, AsyncCommandHandler& handler) :
		pOutstation_ { pOutstation }, handler_(handler), acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), socket_(io_service) {
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
			std::shared_ptr<AsyncCommand> command = handler_.pop();
			std::set<std::shared_ptr<JSONTCPSession>>::iterator it;
			for (it = sessions_.begin(); it != sessions_.end();) {
				if (it->get()->is_active()) {
					it->get()->write(command.get());
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
