#include "Logger.h"
#include <fstream>
#include <iostream>
#include <ctime>

static std::ofstream logFile;
static LogLevel currentLevel = LogLevel::INFO;

void Logger::initialize(const std::string& file) {
    logFile.open(file, std::ios::app);
}

void Logger::log(LogLevel level, const std::string& msg) {
    if (level < currentLevel) return;

    const char* tag =
        level == LogLevel::INFO ? "[INFO]" :
        level == LogLevel::WARNING ? "[WARN]" :
        level == LogLevel::ERROR ? "[ERROR]" : "[DEBUG]";

    std::time_t now = std::time(nullptr);
    std::cout << tag << " " << std::ctime(&now) << " " << msg << "\n";

    if (logFile.is_open()) {
        logFile << tag << " " << std::ctime(&now) << " " << msg << "\n";
    }
}
