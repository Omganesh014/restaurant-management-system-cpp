#pragma once
#include <string>
#include "Common.h"

class Logger {
public:
    static void initialize(const std::string& file = "restaurant.log");
    static void log(LogLevel level, const std::string& message);
};
