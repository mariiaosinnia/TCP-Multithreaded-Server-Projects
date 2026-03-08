#pragma once
#include <queue>
#include <functional>
#include <condition_variable>
#include <mutex>

class ThreadPool {
private:
	std::queue<std::function<void()>> tasks;
	std::mutex mutex;
	std::condition_variable cv;
	std::vector<std::thread> workers;
	int numOfWorkers = 0;
	bool stop = false;
	bool paused = false;

	void start_workers();
	void worker_loop();
	void join_workers();
public:
	~ThreadPool() {
		terminate();
	}
	bool initialize(int workersNum);
	bool terminate();
	bool add_task(std::function<void()> task);
	bool pause();
	bool resume();
};