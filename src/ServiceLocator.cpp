#include "ServiceLocator.h"
#include "Logger.h"
#include <stdexcept>

// Forward declarations + stub implementations
class OrderService {
public:
    void initialize() { Logger::log(LogLevel::INFO, "OrderService initialized"); }
    void shutdown() { Logger::log(LogLevel::INFO, "OrderService shutdown"); }
};

class InventoryService {
public:
    void initialize() { Logger::log(LogLevel::INFO, "InventoryService initialized"); }
    void shutdown() { Logger::log(LogLevel::INFO, "InventoryService shutdown"); }
};

class AnalyticsService {
public:
    void initialize() { Logger::log(LogLevel::INFO, "AnalyticsService initialized"); }
    void shutdown() { Logger::log(LogLevel::INFO, "AnalyticsService shutdown"); }
};

class DeliveryService {
public:
    void initialize() { Logger::log(LogLevel::INFO, "DeliveryService initialized"); }
    void shutdown() { Logger::log(LogLevel::INFO, "DeliveryService shutdown"); }
};

// Static initialization
OrderService* ServiceLocator::orderServiceInstance = nullptr;
InventoryService* ServiceLocator::inventoryServiceInstance = nullptr;
AnalyticsService* ServiceLocator::analyticsServiceInstance = nullptr;
DeliveryService* ServiceLocator::deliveryServiceInstance = nullptr;

void ServiceLocator::initialize() {
    Logger::log(LogLevel::INFO, "ServiceLocator initializing...");
    
    orderServiceInstance = new OrderService();
    inventoryServiceInstance = new InventoryService();
    analyticsServiceInstance = new AnalyticsService();
    deliveryServiceInstance = new DeliveryService();
    
    if (orderServiceInstance) orderServiceInstance->initialize();
    if (inventoryServiceInstance) inventoryServiceInstance->initialize();
    if (analyticsServiceInstance) analyticsServiceInstance->initialize();
    if (deliveryServiceInstance) deliveryServiceInstance->initialize();
    
    Logger::log(LogLevel::INFO, "ServiceLocator ready: 4 services initialized");
}

void ServiceLocator::cleanup() {
    Logger::log(LogLevel::INFO, "ServiceLocator cleaning up...");
    
    if (orderServiceInstance) { orderServiceInstance->shutdown(); delete orderServiceInstance; }
    if (inventoryServiceInstance) { inventoryServiceInstance->shutdown(); delete inventoryServiceInstance; }
    if (analyticsServiceInstance) { analyticsServiceInstance->shutdown(); delete analyticsServiceInstance; }
    if (deliveryServiceInstance) { deliveryServiceInstance->shutdown(); delete deliveryServiceInstance; }
    
    orderServiceInstance = nullptr;
    inventoryServiceInstance = nullptr;
    analyticsServiceInstance = nullptr;
    deliveryServiceInstance = nullptr;
    
    Logger::log(LogLevel::INFO, "ServiceLocator cleanup complete");
}

OrderService& ServiceLocator::order() {
    if (!orderServiceInstance) {
        Logger::log(LogLevel::ERROR, "OrderService not initialized");
        throw std::runtime_error("OrderService not initialized");
    }
    return *orderServiceInstance;
}

InventoryService& ServiceLocator::inventory() {
    if (!inventoryServiceInstance) {
        Logger::log(LogLevel::ERROR, "InventoryService not initialized");
        throw std::runtime_error("InventoryService not initialized");
    }
    return *inventoryServiceInstance;
}

AnalyticsService& ServiceLocator::analytics() {
    if (!analyticsServiceInstance) {
        Logger::log(LogLevel::ERROR, "AnalyticsService not initialized");
        throw std::runtime_error("AnalyticsService not initialized");
    }
    return *analyticsServiceInstance;
}

DeliveryService& ServiceLocator::delivery() {
    if (!deliveryServiceInstance) {
        Logger::log(LogLevel::ERROR, "DeliveryService not initialized");
        throw std::runtime_error("DeliveryService not initialized");
    }
    return *deliveryServiceInstance;
}
