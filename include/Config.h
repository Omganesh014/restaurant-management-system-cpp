#pragma once
#include <string>
#include <map>
#include "Common.h"

/**
 * Configuration System
 * Loads KEY=VALUE pairs from config.txt at startup
 * Enables zero-recompilation tuning of business parameters
 */
class Config {
private:
    static std::map<std::string, std::string> configMap;
    static bool initialized;

public:
    static void initialize(const std::string& configFile = "config/config.txt");
    
    static std::string getString(const std::string& key, const std::string& defaultValue = "");
    static int getInt(const std::string& key, int defaultValue = 0);
    static double getDouble(const std::string& key, double defaultValue = 0.0);
    static bool getBool(const std::string& key, bool defaultValue = false);
    
    static void set(const std::string& key, const std::string& value);
    static bool exists(const std::string& key);
    
    static void logConfiguration();
};
