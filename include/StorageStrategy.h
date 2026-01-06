#ifndef STORAGE_STRATEGY_H
#define STORAGE_STRATEGY_H

#include "Models.h"
#include <string>
#include <vector>
#include <memory>

/**
 * @class StorageStrategy
 * @brief Abstract storage interface (Strategy Pattern)
 * 
 * Decouples business logic from storage implementation.
 * Allows swapping storage backend without code changes.
 * 
 * Current implementations:
 * - CSVStorage (CSV files)
 * - TXTStorage (Text files)
 * 
 * Future implementations:
 * - SQLiteStorage
 * - PostgreSQLStorage
 */
class StorageStrategy {
public:
    virtual ~StorageStrategy() = default;
    
    // Customers
    virtual bool saveCustomer(const CustomerRecord& customer) = 0;
    virtual CustomerRecord loadCustomer(const std::string& id) = 0;
    virtual std::vector<CustomerRecord> loadAllCustomers() = 0;
    virtual bool deleteCustomer(const std::string& id) = 0;
    
    // Menu Items
    virtual bool saveMenuItem(const MenuItem& item) = 0;
    virtual MenuItem loadMenuItem(const std::string& id) = 0;
    virtual std::vector<MenuItem> loadAllMenuItems() = 0;
    virtual bool deleteMenuItem(const std::string& id) = 0;
    
    // Orders
    virtual bool saveOrder(const Order& order) = 0;
    virtual Order loadOrder(const std::string& id) = 0;
    virtual std::vector<Order> loadAllOrders() = 0;
    virtual bool deleteOrder(const std::string& id) = 0;
    
    // Diagnostic
    virtual std::string getName() const = 0;
    virtual bool isHealthy() = 0;
};

/**
 * @class CSVStorageStrategy
 * @brief CSV file-based storage implementation
 */
class CSVStorageStrategy : public StorageStrategy {
public:
    // Customers
    bool saveCustomer(const CustomerRecord& customer) override;
    CustomerRecord loadCustomer(const std::string& id) override;
    std::vector<CustomerRecord> loadAllCustomers() override;
    bool deleteCustomer(const std::string& id) override;
    
    // Menu Items
    bool saveMenuItem(const MenuItem& item) override;
    MenuItem loadMenuItem(const std::string& id) override;
    std::vector<MenuItem> loadAllMenuItems() override;
    bool deleteMenuItem(const std::string& id) override;
    
    // Orders
    bool saveOrder(const Order& order) override;
    Order loadOrder(const std::string& id) override;
    std::vector<Order> loadAllOrders() override;
    bool deleteOrder(const std::string& id) override;
    
    // Diagnostic
    std::string getName() const override { return "CSV Storage"; }
    bool isHealthy() override;
};

/**
 * @class StorageManager
 * @brief Global storage coordinator
 * 
 * Provides single point to configure storage strategy.
 */
class StorageManager {
public:
    static StorageManager& instance();
    
    void setStrategy(std::unique_ptr<StorageStrategy> strategy);
    StorageStrategy& getStrategy();
    std::string getStorageType() const;
    
private:
    StorageManager();
    std::unique_ptr<StorageStrategy> strategy;
};

#endif
