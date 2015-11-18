#include "MessageSystem.h"

void MessageSystem::clean_up() {
	_clean_up = true;

	_work_semaphore.disable();
	
	//Join threads
	for (unsigned int i = 0; i < _threads.size(); ++i) {
		while (_threads[i].joinable()) {
			_threads[i].join();
		}
	}
}

void MessageSystem::thread_func(int id) {
	while (true) {
		// Wait for work notification
		_work_semaphore.wait();

		// Stop if stopping
		if (_clean_up) {
			break;
		}

		// Create an empty message pair
		MessagePair mp;

		// Fetch work
		_request_mutex.lock();
		mp = _request_queue.front();
		_request_queue.pop();
		_request_mutex.unlock();

		// Process message
		if (mp._message != nullptr) {
			mp._message->process();
			_done_mutex.lock();
			_done_collection.insert({ mp._id, mp });
			_done_mutex.unlock();
		}
	}
}

void MessageSystem::initialize() {
	//Use almost all of the hardware threads, using all causes some lag to occur
	_hardware_concurrency = std::thread::hardware_concurrency();
	if (_hardware_concurrency >= 2) {
		--_hardware_concurrency;
	}
	//Start thread functions
	for (int i = 0; i < _hardware_concurrency; ++i) {
		_threads.push_back(std::thread(&MessageSystem::thread_func, this, i));
	}
}

int MessageSystem::post(std::shared_ptr<Message> message) {
	//Message identifier
	static int id = 0;
	//Add a request
	_request_mutex.lock();
	_request_queue.push(MessagePair(message, ++id));
	_request_mutex.unlock();

	// Notify workers
	_work_semaphore.notify();

	return id;
}

/**
 * If the argument message exists, return the data
 */
std::shared_ptr<Message> MessageSystem::get(int id) {
	std::shared_ptr<Message> message = nullptr;

	_done_mutex.lock();
	auto it = _done_collection.find(id);
	if (it != _done_collection.end()) {
		//Remove it from the done collection
		message = (*it).second._message;
		_done_collection.erase(id);
	}
	_done_mutex.unlock();

	return message;
}