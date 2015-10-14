#include <queue>
#include <condition_variable>

#include "AsyncCommand.cpp"

class AsyncCommandQueue {
public:
	/**
	 * Blocks on queue until one or more AsyncCommand elements are available.
	 * TODO Handle more than 1 subscriber
	 */
	std::shared_ptr<AsyncCommand> pop() {
		std::unique_lock<std::mutex> lock(queue_mutex_);
		while (queue_.empty()) {
			queue_cond_.wait(lock);
		}
		std::shared_ptr<AsyncCommand> command = queue_.front();
		queue_.pop();
		return command;
	}

	void push(AsyncCommand* command) {
		std::unique_lock<std::mutex> lock(queue_mutex_);
		queue_.push(std::shared_ptr<AsyncCommand>(command));
		lock.unlock();
		queue_cond_.notify_all();
	}

private:
	std::queue<std::shared_ptr<AsyncCommand>> queue_;
	std::mutex queue_mutex_;
	std::condition_variable queue_cond_;
};
