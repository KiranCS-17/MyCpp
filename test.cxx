#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstdlib>  // For random failure simulation

// Function to simulate sending a request, with a possibility of failure
bool sendRequest(const std::string &request) {
    std::cout << "Sending request: " << request << std::endl;

    // Simulate network delay or server processing
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Simulate random failure (e.g., 30% chance of failure)
    if (rand() % 10 < 3) {
        std::cout << "Request failed: " << request << std::endl;
        return false;  // Indicate failure
    }

    std::cout << "Request succeeded: " << request << std::endl;
    return true;  // Indicate success
}

// Function to process a batch of requests
void processBatch(std::queue<std::string> &requestQueue, int batchSize, int interval, std::mutex &queueMutex) {
    while (true) {
        std::lock_guard<std::mutex> lock(queueMutex);

        if (requestQueue.empty()) {
            break; // Exit if no more requests
        }

        // Process up to batchSize requests
        for (int i = 0; i < batchSize && !requestQueue.empty(); ++i) {
            std::string request = requestQueue.front();
            requestQueue.pop();

            // Try sending the request, and if it fails, push it back to the queue for retry
            if (!sendRequest(request)) {
                std::cout << "Retrying request: " << request << std::endl;
                requestQueue.push(request);  // Push back to the queue for retry
            }
        }

        // Wait for the next interval before processing the next batch
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
}

int main() {
    std::queue<std::string> requestQueue;
    std::mutex queueMutex;

    // Simulate filling the queue with requests
    for (int i = 0; i < 100; ++i) {
        requestQueue.push("Request " + std::to_string(i));
    }

    // Parameters for batch processing
    int batchSize = 5;            // Number of requests to send per batch
    int interval = 2000;          // Interval between batches in milliseconds (2 seconds)

    // Start processing batches in a separate thread
    std::thread worker(processBatch, std::ref(requestQueue), batchSize, interval, std::ref(queueMutex));

    // Wait for the worker to finish processing all requests
    worker.join();

    std::cout << "All requests processed." << std::endl;

    return 0;
}
