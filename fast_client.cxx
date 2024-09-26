#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <chrono>
#include <curl/curl.h>
#include <algorithm>
const int THREAD_POOL_SIZE = 32;  // Number of threads in the pool
const int REQUESTS_PER_SECOND = 100;  // Target number of requests per second
const int TOTAL_REQUESTS = 10000;  // Total number of requests to send

// Callback function to capture the response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // Calculate the real size of the incoming buffer
    size_t totalSize = size * nmemb;

    // Cast the user pointer to a string and append the incoming data
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), totalSize);

    // Return the number of bytes processed
    return totalSize;
}

// Function to send a request to the server
void sendRequest(int request_id) {
    CURL *curl;
    CURLcode res;
    std::string responseString;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:30007");
        
        // Set timeout for the request
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
	       // Set the callback function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        
        // Set the pointer to the string where the response will be stored
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
        
        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        
        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "Request " << request_id << " failed: "
                      << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "Request " << request_id << " succeeded." << std::endl;
	     std::cout << "Response: " << responseString << std::endl;
        }

        // Cleanup curl
        curl_easy_cleanup(curl);
    }
}

// Function to manage the thread pool and send requests
void sendRequestsWithThreadPool() {
    // Vector of futures to store async results
    std::vector<std::future<void>> futures;

    int request_counter = 0;

    // Start a loop that sends 1000 requests per second
    while (request_counter < TOTAL_REQUESTS) {
        auto start = std::chrono::high_resolution_clock::now();

        // Send REQUESTS_PER_SECOND requests using the thread pool
        for (int i = 0; i < REQUESTS_PER_SECOND; ++i) {
            if (request_counter >= TOTAL_REQUESTS) break;
            
            // Add a new task to the thread pool
            futures.push_back(std::async(std::launch::async, sendRequest, request_counter));
            request_counter++;
        }

        // Sleep to ensure we don't exceed 1000 requests per second
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        if (elapsed.count() < 1.0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(1.0 - elapsed.count()));
        }

        // Clean up finished futures
        futures.erase(
            std::remove_if(futures.begin(), futures.end(),
                [](std::future<void> &fut) { return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }),
            futures.end());
    }

    // Wait for all remaining requests to finish
    for (auto &fut : futures) {
        fut.wait();
    }
}

int main() {
    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);

    // Send requests with thread pool
    sendRequestsWithThreadPool();

    // Clean up curl
    curl_global_cleanup();

    return 0;
}

