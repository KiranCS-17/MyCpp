#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <mutex>
#include <sstream>
#include <sys/stat.h> // For checking file size
#include <iomanip>    // For formatting time

// Log Levels
enum LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger {
private:
    LogLevel currentLevel;         // Current logging level
    std::ofstream logFile;         // File stream for logging to a file
    std::string logFilePath;       // Log file path
    bool toFile;                   // Flag to indicate if logging to a file
    std::mutex logMutex;           // Mutex for thread safety
    size_t maxFileSize;            // Maximum log file size in bytes (for rotation)

    // Function to get current timestamp (declared and defined outside the class)
    std::string getTimestamp();

    // Log file rotation function (declared and defined outside the class)
    void rotateLogFile();

public:
    // Constructor with log level, file path, and max file size (in bytes) for log rotation
    Logger(LogLevel level = INFO, const std::string& filePath = "", size_t maxSize = 1024 * 1024)
        : currentLevel(level), logFilePath(filePath), maxFileSize(maxSize) {
        toFile = !logFilePath.empty();
        if (toFile) {
            logFile.open(logFilePath, std::ios::app); // Open file in append mode
            if (!logFile.is_open()) {
                std::cerr << "Failed to open log file!" << std::endl;
                toFile = false; // Fallback to console logging
            }
        }
    }

    // Destructor to close the file
    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    // Template function for logging various data types
    template <typename T>
    void log(LogLevel level, T message, const std::string& fileName = "", int lineNumber = 0) {
        if (level >= currentLevel) {
            std::lock_guard<std::mutex> guard(logMutex); // Lock mutex for thread safety

            // Format the log message
            std::ostringstream logStream;
            logStream << getTimestamp() << " [" << levelToString(level) << "] ";
            if (!fileName.empty()) {
                logStream << fileName << ":" << lineNumber << " "; // Add file name and line number
            }
            logStream << message;

            if (toFile) {
                rotateLogFile(); // Check and rotate log file if necessary
                logFile << logStream.str() << std::endl;
            } else {
                std::cout << logStream.str() << std::endl;
            }
        }
    }

    // Helper to convert log level to string
    std::string levelToString(LogLevel level) {
        switch (level) {
            case INFO: return "INFO";
            case WARNING: return "WARNING";
            case ERROR: return "ERROR";
            case DEBUG: return "DEBUG";
            default: return "UNKNOWN";
        }
    }

    // Template specialization for logging time_t type
    void log(LogLevel level, time_t timeValue, const std::string& fileName = "", int lineNumber = 0) {
        std::lock_guard<std::mutex> guard(logMutex); // Lock mutex for thread safety

        // Convert time_t to readable time string
        std::ostringstream logStream;
        logStream << getTimestamp() << " [" << levelToString(level) << "] ";
        if (!fileName.empty()) {
            logStream << fileName << ":" << lineNumber << " "; // Add file name and line number
        }

        // Convert time_t to local time string
        std::tm* ptm = std::localtime(&timeValue);
        char buffer[32];
        std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
        logStream << "Time: " << buffer;

        if (toFile) {
            rotateLogFile(); // Check and rotate log file if necessary
            logFile << logStream.str() << std::endl;
        } else {
            std::cout << logStream.str() << std::endl;
        }
    }
};

// Get current timestamp (defined outside the class)
std::string Logger::getTimestamp() {
    std::time_t now = std::time(0);
    char buf[80];
    std::tm *timeinfo = std::localtime(&now);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buf);
}

// Rotate log file if it exceeds the maximum file size (defined outside the class)
void Logger::rotateLogFile() {
    // Check current file size
    struct stat fileStat;
    if (stat(logFilePath.c_str(), &fileStat) == 0 && fileStat.st_size >= maxFileSize) {
        logFile.close(); // Close the current file

        // Rename the current log file by appending timestamp
        std::string newFileName = logFilePath + "." + getTimestamp();
        std::rename(logFilePath.c_str(), newFileName.c_str());

        // Open a new log file
        logFile.open(logFilePath, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Failed to open new log file after rotation!" << std::endl;
        }
    }
}

// Macro to simplify logging with file name and line number
#define LOG(logger, level, message) logger.log(level, message, __FILE__, __LINE__)

