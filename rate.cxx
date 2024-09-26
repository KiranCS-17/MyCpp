#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>

class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();

    // Add a request (task) to the queue
    void enqueue_task(std::function<void()> task);

private:
    // Worker threads that execute tasks
    std::vector<std::thread> workers;

    // Task queue to buffer requests
    std::queue<std::function<void()>> tasks;

    // Synchronization mechanisms
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

    // Function run by each thread to fetch and execute tasks
    void worker_thread();
};

// ThreadPool constructor that creates worker threads
ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] { worker_thread(); });
    }
}

// Destructor that joins all threads
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

// Function that enqueues a task into the queue
void ThreadPool::enqueue_task(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push(std::move(task));
    }
    condition.notify_one();
}

// Function run by each worker thread
void ThreadPool::worker_thread() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // Wait until there is a task in the queue or the pool is stopped
            condition.wait(lock, [this] { return !tasks.empty() || stop; });

            if (stop && tasks.empty()) {
                return;
            }

            // Get the next task from the queue
            task = std::move(tasks.front());
            tasks.pop();
        }

        // Execute the task
        task();
    }
}

// Simulated request processing function
void process_request(int request_id) {
    std::cout << "Processing request " << request_id << " on thread " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Simulate work
}

int main() {
    const int num_threads = 4;  // Limit number of threads in the pool
    ThreadPool pool(num_threads);

    // Enqueue 10 requests
    for (int i = 1; i <= 10; ++i) {
        pool.enqueue_task([i] {
            process_request(i);
        });
    }

    // Wait for some time to let threads finish (just for this demo)
    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}

