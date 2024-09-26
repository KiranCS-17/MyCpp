#include "Logger.h"
int main()
{

    // Create a logger instance that logs to a file with a max size of 1 MB
    Logger fileLogger(INFO, "app.log", 1024 * 1024);
    // Logging to file with rotation
    LOG(fileLogger, INFO, "This is an info message.");
    LOG(fileLogger, WARNING, 33);
    LOG(fileLogger, ERROR, "This is an error message.");
    LOG(fileLogger, DEBUG, "This is a debug message.");

    // Create another logger instance for console logging
    Logger consoleLogger(INFO);
    LOG(consoleLogger, INFO, "This is a console info message.");
    LOG(consoleLogger, DEBUG, "This debug message will not be printed (current level is INFO).");

    return 0;

}
