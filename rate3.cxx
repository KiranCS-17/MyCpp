#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <boost/asio.hpp>
#include<string>
#include <chrono>

int num_requests = 1000;
std::string server = "127.0.0.1"; // Replace with your server IP
std::string port = "80";          // Replace with your server port

using boost::asio::ip::tcp;
boost::asio::io_context io_context;
class ThreadPool
{
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();

    // Add a request (task) to the queue
    void enqueue_task(std::function<void(boost::asio::io_context,std::string,std::string ,int)> task);

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
ThreadPool::ThreadPool(size_t num_threads) : stop(false)
{
    for (size_t i = 0; i < num_threads; ++i)
    {
        workers.emplace_back([this]
                             { worker_thread(); });
    }
}

// Destructor that joins all threads
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers)
    {
        worker.join();
    }
}

// Function that enqueues a task into the queue
void ThreadPool::enqueue_task(std::function<void(void(boost::asio::io_context,std::string,std::string ,int)> task)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push(std::move(task));
    }
    condition.notify_one();
}

// Function run by each worker thread
void ThreadPool::worker_thread()
{
    while (true)
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // Wait until there is a task in the queue or the pool is stopped
            condition.wait(lock, [this]
                           { return !tasks.empty() || stop; });

            if (stop && tasks.empty())
            {
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

void send_request(boost::asio::io_context &io_context, const std::string &server, const std::string &port, int request_id)
{
    try
    {
        // Create a TCP resolver and socket
        tcp::resolver resolver(io_context);
        tcp::socket socket(io_context);

        // Resolve the server and port
        boost::asio::connect(socket, resolver.resolve(server, port));

        // Build the request (simple HTTP GET in this case)
        std::string request = "GET / HTTP/1.1\r\nHost: " + server + "\r\n\r\n";

        // Send the request
        boost::asio::write(socket, boost::asio::buffer(request));

        // Receive the response (optional)
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        std::cout << "Request " << request_id << " sent and response received.\n";
    }
    catch (std::exception &e)
    {
        std::cerr << "Error on request " << request_id << ": " << e.what() << "\n";
    }
}

// Function to manage sending requests in parallel
void send_requests_concurrently(int num_requests,  std::string server,  std::string port)
{
    // Create a Boost.Asio context

    // Vector to hold threads
    const int num_threads = 4; // Limit number of threads in the pool
    ThreadPool pool(num_threads);

    // Start timer
    auto start_time = std::chrono::high_resolution_clock::now();

    // Spawn threads for each request
    for (int i = 0; i < num_requests; ++i)
    {
        pool.enqueue_task([i]
                          { send_request(io_context, server, port, i); });
    }

    // End timer and calculate elapsed time
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "All requests sent in: " << elapsed.count() << " seconds\n";
}

int main()
{

    send_requests_concurrently(num_requests, server, port);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
