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

		MessagePair mp = pop_request();

		// Process message
		if (mp._message != nullptr) {
			mp._message->process();
		}

		push_done(mp);
	}
}

int MessageSystem::push_request(std::shared_ptr<Message> message, bool noreturn) {
	std::unique_lock<std::mutex> lock(_request_mutex);

	// Increase counter
	int id = ++_id;
	//Add a request
	_request_queue.push(MessagePair(message, id, noreturn));
	// Notify workers
	_work_semaphore.notify();

	return id;
}

MessagePair MessageSystem::pop_request() {
	std::unique_lock<std::mutex> lock(_request_mutex);

	auto message = _request_queue.front();
	_request_queue.pop();
	return message;
}

void MessageSystem::push_done(MessagePair message) {
	std::unique_lock<std::mutex> lock(_done_mutex);

	_done_collection.insert({ message._id, message });
}

std::shared_ptr<Message> MessageSystem::pop_done(int id) {
	std::unique_lock<std::mutex> lock(_done_mutex);

	auto it = _done_collection.find(id);
	if (it != _done_collection.end()) {
		//Remove it from the done collection
		auto message = (*it).second._message;
		_done_collection.erase(id);
		return message;
	}
	return nullptr;
}

void MessageSystem::initialize() {
	_id = 0;
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
	return push_request(message, false);
}

void MessageSystem::post_noreturn(std::shared_ptr<Message> message) {
	push_request(message, true);
}

/**
 * If the argument message exists, return the data
 */
std::shared_ptr<Message> MessageSystem::get(int id) {
	return pop_done(id);
}