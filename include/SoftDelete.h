#pragma once
#include "Models.h"
#include <string>
#include <vector>
#include <ctime>

/**
 * Soft Delete System
 * Instead of physically removing records, mark them inactive
 * Preserves data integrity, audit trail, referential links
 * Standard in production systems for compliance/recovery
 */

// Extend Customer with soft delete fields
struct CustomerRecord {
    int id;
    std::string name;
    std::string phone;
    std::string email;
    int loyaltyPoints;
    
    // Soft delete fields
    bool isActive = true;
    std::time_t deletedAt = 0;  // 0 = not deleted
    std::string deletionReason = "";  // Why was it deleted?
    
    bool isDeleted() const { return !isActive; }
    void softDelete(const std::string& reason = "") {
        isActive = false;
        deletedAt = std::time(nullptr);
        deletionReason = reason;
    }
    void restore() {
        isActive = true;
        deletedAt = 0;
        deletionReason = "";
    }
};

// Extend MenuItem with soft delete fields
struct MenuItemRecord {
    int id;
    std::string name;
    std::string category;
    double price;
    
    // Soft delete fields
    bool isActive = true;
    std::time_t deletedAt = 0;
    std::string deletionReason = "";
    
    bool isDeleted() const { return !isActive; }
    void softDelete(const std::string& reason = "") {
        isActive = false;
        deletedAt = std::time(nullptr);
        deletionReason = reason;
    }
    void restore() {
        isActive = true;
        deletedAt = 0;
        deletionReason = "";
    }
};

// Extend Order with soft delete fields
struct OrderRecord {
    int orderId;
    int customerId;
    double total;
    int priority;
    std::time_t timestamp;
    OrderState state;
    
    // Soft delete fields
    bool isActive = true;
    std::time_t deletedAt = 0;
    std::string cancellationReason = "";
    
    bool isDeleted() const { return !isActive; }
    void softDelete(const std::string& reason = "") {
        isActive = false;
        deletedAt = std::time(nullptr);
        cancellationReason = reason;
    }
    void restore() {
        isActive = true;
        deletedAt = 0;
        cancellationReason = "";
    }
};

/**
 * Soft Delete Repository
 * Base class for any entity with soft delete
 */
template <typename T>
class SoftDeleteRepository {
public:
    virtual ~SoftDeleteRepository() = default;
    
    // Get only active records
    virtual std::vector<T> getActive() = 0;
    
    // Get all records (including soft-deleted)
    virtual std::vector<T> getAll() = 0;
    
    // Get by ID (only if active)
    virtual T* findActive(int id) = 0;
    
    // Get by ID (even if soft-deleted)
    virtual T* findAny(int id) = 0;
    
    // Count active records
    virtual int countActive() = 0;
    
    // Soft delete (mark inactive)
    virtual bool softDelete(int id, const std::string& reason = "") = 0;
    
    // Restore (mark active again)
    virtual bool restore(int id) = 0;
};
