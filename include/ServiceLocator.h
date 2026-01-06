#pragma once
#include <stdexcept>

// Forward declarations
class OrderService;
class InventoryService;
class AnalyticsService;
class DeliveryService;

/**
 * Service Locator Pattern
 * Central registry for all service instances
 * Single source of truth for service access
 * Enables dependency injection and easier testing
 */
class ServiceLocator {
private:
    static OrderService* orderServiceInstance;
    static InventoryService* inventoryServiceInstance;
    static AnalyticsService* analyticsServiceInstance;
    static DeliveryService* deliveryServiceInstance;

public:
    // Initialize all services
    static void initialize();
    
    // Cleanup all services
    static void cleanup();
    
    // Service accessors
    static OrderService& order();
    static InventoryService& inventory();
    static AnalyticsService& analytics();
    static DeliveryService& delivery();
};
