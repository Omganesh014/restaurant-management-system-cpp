#include "IdempotencyService.h"
#include "Logger.h"

std::map<std::string, IdempotencyRecord> IdempotencyService::registry;
int IdempotencyService::defaultTTLSeconds = 86400;  // 24 hours

bool IdempotencyService::isDuplicate(const std::string& requestId, std::string& cachedResult) {
    auto it = registry.find(requestId);
    
    if (it == registry.end()) {
        // Not seen before
        return false;
    }
    
    IdempotencyRecord& record = it->second;
    
    // Check if expired
    if (record.isExpired()) {
        Logger::log(LogLevel::DEBUG, "IdempotencyService: Request " + requestId + " record expired");
        registry.erase(it);
        return false;
    }
    
    // Return cached result
    cachedResult = record.resultData;
    Logger::log(LogLevel::INFO, "IdempotencyService: Duplicate request " + requestId + 
               " detected, returning cached result");
    return true;
}

void IdempotencyService::recordSuccess(const std::string& requestId, 
                                      const std::string& operationType,
                                      const std::string& resultData) {
    registry[requestId] = {
        requestId,
        operationType,
        true,
        resultData,
        std::time(nullptr),
        defaultTTLSeconds
    };
    Logger::log(LogLevel::INFO, "IdempotencyService: Recorded success for " + requestId);
}

void IdempotencyService::recordFailure(const std::string& requestId, 
                                      const std::string& operationType) {
    registry[requestId] = {
        requestId,
        operationType,
        false,
        "",
        std::time(nullptr),
        defaultTTLSeconds
    };
    Logger::log(LogLevel::WARNING, "IdempotencyService: Recorded failure for " + requestId);
}

IdempotencyRecord* IdempotencyService::getRecord(const std::string& requestId) {
    auto it = registry.find(requestId);
    if (it != registry.end()) {
        return &(it->second);
    }
    return nullptr;
}

void IdempotencyService::cleanupExpired() {
    int removed = 0;
    auto it = registry.begin();
    while (it != registry.end()) {
        if (it->second.isExpired()) {
            it = registry.erase(it);
            removed++;
        } else {
            ++it;
        }
    }
    if (removed > 0) {
        Logger::log(LogLevel::INFO, "IdempotencyService: Cleaned up " + std::to_string(removed) + 
                   " expired records");
    }
}

int IdempotencyService::getTrackedCount() {
    return static_cast<int>(registry.size());
}

void IdempotencyService::setDefaultTTL(int seconds) {
    defaultTTLSeconds = seconds;
    Logger::log(LogLevel::INFO, "IdempotencyService: Default TTL set to " + std::to_string(seconds) + "s");
}
