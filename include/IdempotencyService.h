#pragma once
#include <string>
#include <map>
#include <ctime>

/**
 * Idempotent Operations System
 * Prevent duplicate actions using request IDs
 * If same request ID seen before → return cached result
 * Essential for production reliability (network retries, etc.)
 */

struct IdempotencyRecord {
    std::string requestId;
    std::string operationType;  // "place_order", "process_payment", etc.
    bool succeeded;
    std::string resultData;     // Serialized result to return
    std::time_t createdAt;
    int ttlSeconds = 86400;     // 24 hours default
    
    bool isExpired() const {
        std::time_t now = std::time(nullptr);
        return (now - createdAt) > ttlSeconds;
    }
};

/**
 * Idempotency Service
 * Track request IDs and their outcomes
 * Return cached result if duplicate request detected
 */
class IdempotencyService {
private:
    static std::map<std::string, IdempotencyRecord> registry;
    static int defaultTTLSeconds;

public:
    /**
     * Check if request was already processed
     * Returns cached result if found and not expired
     */
    static bool isDuplicate(const std::string& requestId, std::string& cachedResult);
    
    /**
     * Record a successful operation result
     */
    static void recordSuccess(const std::string& requestId, 
                            const std::string& operationType,
                            const std::string& resultData);
    
    /**
     * Record a failed operation
     */
    static void recordFailure(const std::string& requestId, 
                            const std::string& operationType);
    
    /**
     * Get operation record (for audit/replay)
     */
    static IdempotencyRecord* getRecord(const std::string& requestId);
    
    /**
     * Clear expired records (call periodically)
     */
    static void cleanupExpired();
    
    /**
     * Get count of tracked requests
     */
    static int getTrackedCount();
    
    /**
     * Set default TTL for new records
     */
    static void setDefaultTTL(int seconds);
};
