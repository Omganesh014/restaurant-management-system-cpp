#include "Config.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>

std::map<std::string, std::string> Config::configMap;
bool Config::initialized = false;

void Config::initialize(const std::string& configFile) {
    if (initialized) return;
    
    std::ifstream file(configFile);
    if (!file.is_open()) {
        Logger::log(LogLevel::WARNING, "Config file not found: " + configFile + ", using defaults");
        initialized = true;
        return;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        // Parse KEY=VALUE
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            Logger::log(LogLevel::WARNING, "Invalid config line " + std::to_string(lineNum) + ": " + line);
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Trim whitespace
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        configMap[key] = value;
    }
    
    file.close();
    initialized = true;
    Logger::log(LogLevel::INFO, "Configuration loaded: " + std::to_string(configMap.size()) + " settings");
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) {
    auto it = configMap.find(key);
    return (it != configMap.end()) ? it->second : defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) {
    auto it = configMap.find(key);
    if (it == configMap.end()) return defaultValue;
    try {
        return std::stoi(it->second);
    } catch (...) {
        Logger::log(LogLevel::WARNING, "Invalid integer config value for " + key);
        return defaultValue;
    }
}

double Config::getDouble(const std::string& key, double defaultValue) {
    auto it = configMap.find(key);
    if (it == configMap.end()) return defaultValue;
    try {
        return std::stod(it->second);
    } catch (...) {
        Logger::log(LogLevel::WARNING, "Invalid double config value for " + key);
        return defaultValue;
    }
}

bool Config::getBool(const std::string& key, bool defaultValue) {
    auto it = configMap.find(key);
    if (it == configMap.end()) return defaultValue;
    std::string val = it->second;
    std::transform(val.begin(), val.end(), val.begin(), ::tolower);
    return (val == "true" || val == "yes" || val == "1");
}

void Config::set(const std::string& key, const std::string& value) {
    configMap[key] = value;
}

bool Config::exists(const std::string& key) {
    return configMap.find(key) != configMap.end();
}

void Config::logConfiguration() {
    Logger::log(LogLevel::INFO, "=== LOADED CONFIGURATION ===");
    for (const auto& pair : configMap) {
        Logger::log(LogLevel::DEBUG, pair.first + " = " + pair.second);
    }
    Logger::log(LogLevel::INFO, "=============================");
}
