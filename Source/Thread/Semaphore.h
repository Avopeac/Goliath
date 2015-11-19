#pragma once
#include <mutex>

/**
 * See http://stackoverflow.com/a/19659736
*/

class Semaphore
{
public:
	Semaphore(int count_ = 0)
		: count(count) {
	}

	inline void notify() {
		std::unique_lock<std::mutex> lock(mtx);
		count++;
		cv.notify_one();
	}

	inline void wait() {
		std::unique_lock<std::mutex> lock(mtx);
		while (count == 0) {
			cv.wait(lock);
		}
		count--;
	}

	inline void disable() {
		std::unique_lock<std::mutex> lock(mtx);
		count = INT_MAX;
		cv.notify_all();
	}

private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;
};