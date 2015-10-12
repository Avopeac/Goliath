#pragma once
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <queue>
#include <memory>
#include "Message.h"
///A message pair is used to identify a message after processing
typedef struct MessagePair {
	MessagePair(std::shared_ptr<Message> message = nullptr, int id = 0) : _message(message), _id(id) {}
	std::shared_ptr<Message> _message;
	int _id;
} MessagePair;

///A message system is a job scheduler for the available hardware threads. The threads run a looping function which picks work from a shared queue.
///When the work that is defined by the type of message is processed, it puts the processed message into a collection of processed messages.
class MessageSystem {
public:
	//Singleton
	static MessageSystem &instance() {
		static MessageSystem instance;
		return instance;
	}
	//Post a message to the request queue, returns the id of the message pair
	int post(std::shared_ptr<Message> message);
	//Get a message from the done collection, null if no such message exists
	std::shared_ptr<Message> get(int id);
	//Shut down the threads
	void clean_up();

private:
	//Private constructor for singleton
	MessageSystem() { initialize(); }
	//Creates threads and starts their looping functions
	void initialize();
	//The looping function that picks work
	void thread_func(int id);
	//How many hardware threads that will be used
	int _hardware_concurrency;
	//Need to clean up, exit condition for threads
	bool _clean_up = false;

	std::vector<std::thread> _threads;
	std::queue<MessagePair> _request_queue;
	std::map<int, MessagePair> _done_collection;

	//Locks for shared data-structures
	std::mutex _request_mutex;
	std::mutex _done_mutex;
};
