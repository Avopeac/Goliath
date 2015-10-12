#include "MessageSystem.h"

void MessageSystem::clean_up() {
	_clean_up = true;
	//Join threads
	for (int i = 0; i < _threads.size(); ++i) {
		if (_threads[i].joinable()) {
			_threads[i].join();
		}
	}
}

void MessageSystem::thread_func(int id) {
	bool working = false;
	//Loop continuously
	while (!_clean_up) {
		//Create an empty message pair
		MessagePair mp;
		if (!working) {
			//Request work if not working
			_request_mutex.lock();
			if (_request_queue.size() > 0) {
				mp = _request_queue.front();
				_request_queue.pop();
				working = true;
			}
			_request_mutex.unlock();
		}
		//Process message
		if (mp._message != nullptr) {
			mp._message->process();
			_done_mutex.lock();
			_done_collection.insert({ mp._id, mp });
			_done_mutex.unlock();
			working = false;
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
	return id;
}

std::shared_ptr<Message> MessageSystem::get(int id) {
	//If the argument message exists, return the data
	_done_mutex.lock();
	auto it = _done_collection.find(id);
	std::shared_ptr<Message> message = nullptr;
	if (it != _done_collection.end()) {
		//Remove it from the done collection
		message = (*it).second._message;
		_done_collection.erase(id);
	}
	_done_mutex.unlock();
	return message;
}