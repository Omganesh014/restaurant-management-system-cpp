#pragma once
#include <string>
#include <vector>
#include <ctime>

/**
 * Event System (Observer Pattern)
 * Backend emits strongly-typed events
 * Decouples order service from logging, analytics, audit trail
 */

enum class EventType {
    ORDER_PLACED,
    ORDER_CONFIRMED,
    ORDER_PREPARING,
    ORDER_READY,
    ORDER_SERVED,
    ORDER_CANCELLED,
    ORDER_REFUNDED,
    INVENTORY_UPDATED,
    INVENTORY_LOW,
    CUSTOMER_CREATED,
    CUSTOMER_DELETED,
    PAYMENT_PROCESSED,
    REFUND_ISSUED
};

struct Event {
    EventType type;
    int entityId;          // orderId, customerId, itemId, etc.
    std::string entityType; // "Order", "Customer", "Inventory", etc.
    std::string details;   // JSON-like or freeform context
    std::time_t timestamp;
    std::string sourceAction; // Which service emitted this
};

/**
 * Event listener interface
 * Implement to react to events
 */
class EventListener {
public:
    virtual ~EventListener() = default;
    virtual void onEvent(const Event& event) = 0;
    virtual std::string getName() const = 0;
};

/**
 * Event bus - central dispatcher
 * Loosely couples services via publish-subscribe
 */
class EventBus {
private:
    static EventBus* instance;
    std::vector<EventListener*> listeners;

public:
    static EventBus& getInstance();
    
    // Register a listener
    void subscribe(EventListener* listener);
    
    // Unregister a listener
    void unsubscribe(EventListener* listener);
    
    // Emit an event to all listeners
    void emit(const Event& event);
    
    // Cleanup
    void clear();
};
