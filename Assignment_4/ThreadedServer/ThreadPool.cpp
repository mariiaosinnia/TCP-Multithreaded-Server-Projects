#include "ThreadPool.h"

void ThreadPool::start_workers() {
    for (int i = 0; i < numOfWorkers; i++) {
        workers.emplace_back(&ThreadPool::worker_loop, this);
    }
}

void ThreadPool::worker_loop() {
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this] {
                return stop || (!paused && !tasks.empty());
                });

            if (stop && tasks.empty()) {
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}

void ThreadPool::join_workers() {
    for (auto& worker : workers)
    {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

bool ThreadPool::initialize(int inNumOfWorkers) {
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!workers.empty() || inNumOfWorkers <= 0) {
            return false;
        }

        stop = false;
        paused = false;

        numOfWorkers = inNumOfWorkers;
    }
    start_workers();
    return true;
}

bool ThreadPool::terminate() {
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (stop) {
            return true;
        }

        stop = true;
    }

    cv.notify_all();
    join_workers();
    {
        std::lock_guard<std::mutex> lock(mutex);
        workers.clear();
    }
    return true;
}

bool ThreadPool::add_task(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(mutex);

    if (stop || workers.empty()) {
        return false;
    }

    tasks.push(std::move(task));
    cv.notify_one();
    return true;
}

bool ThreadPool::pause() {
    std::lock_guard<std::mutex> lock(mutex);

    if (stop) {
        return false;
    }

    paused = true;
    return true;
}

bool ThreadPool::resume() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (stop) {
            return false;
        }

        paused = false;
    }

    cv.notify_all();
    return true;
}