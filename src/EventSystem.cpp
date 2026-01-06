#include "EventSystem.h"
#include "Logger.h"
#include <algorithm>
#include <iostream>

EventBus* EventBus::instance = nullptr;

EventBus& EventBus::getInstance() {
    if (!instance) {
        instance = new EventBus();
    }
    return *instance;
}

void EventBus::subscribe(EventListener* listener) {
    if (!listener) return;
    
    // Avoid duplicates
    auto it = std::find(listeners.begin(), listeners.end(), listener);
    if (it == listeners.end()) {
        listeners.push_back(listener);
        Logger::log(LogLevel::INFO, "EventBus: Listener '" + listener->getName() + "' subscribed");
    }
}

void EventBus::unsubscribe(EventListener* listener) {
    if (!listener) return;
    
    auto it = std::find(listeners.begin(), listeners.end(), listener);
    if (it != listeners.end()) {
        listeners.erase(it);
        Logger::log(LogLevel::INFO, "EventBus: Listener '" + listener->getName() + "' unsubscribed");
    }
}

void EventBus::emit(const Event& event) {
    std::string eventName;
    switch (event.type) {
        case EventType::ORDER_PLACED:       eventName = "ORDER_PLACED"; break;
        case EventType::ORDER_CONFIRMED:    eventName = "ORDER_CONFIRMED"; break;
        case EventType::ORDER_PREPARING:    eventName = "ORDER_PREPARING"; break;
        case EventType::ORDER_READY:        eventName = "ORDER_READY"; break;
        case EventType::ORDER_SERVED:       eventName = "ORDER_SERVED"; break;
        case EventType::ORDER_CANCELLED:    eventName = "ORDER_CANCELLED"; break;
        case EventType::ORDER_REFUNDED:     eventName = "ORDER_REFUNDED"; break;
        case EventType::INVENTORY_UPDATED:  eventName = "INVENTORY_UPDATED"; break;
        case EventType::INVENTORY_LOW:      eventName = "INVENTORY_LOW"; break;
        case EventType::CUSTOMER_CREATED:   eventName = "CUSTOMER_CREATED"; break;
        case EventType::CUSTOMER_DELETED:   eventName = "CUSTOMER_DELETED"; break;
        case EventType::PAYMENT_PROCESSED:  eventName = "PAYMENT_PROCESSED"; break;
        case EventType::REFUND_ISSUED:      eventName = "REFUND_ISSUED"; break;
        default:                            eventName = "UNKNOWN"; break;
    }
    
    Logger::log(LogLevel::DEBUG, "EventBus: Emitting " + eventName + " (entity:" + event.entityType + 
                                 "#" + std::to_string(event.entityId) + ")");
    
    // Dispatch to all listeners
    for (auto listener : listeners) {
        try {
            listener->onEvent(event);
        } catch (const std::exception& e) {
            Logger::log(LogLevel::ERROR, "EventBus: Listener '" + listener->getName() + 
                       "' threw exception: " + std::string(e.what()));
        }
    }
}

void EventBus::clear() {
    listeners.clear();
    Logger::log(LogLevel::INFO, "EventBus: Cleared all listeners");
}

// ============================================================================
// Sample Listeners
// ============================================================================

class LoggerListener : public EventListener {
public:
    void onEvent(const Event& event) override {
        std::string eventName = "[EVENT]";
        Logger::log(LogLevel::INFO, eventName + " " + event.entityType + 
                   "#" + std::to_string(event.entityId) + " | " + event.details);
    }
    std::string getName() const override { return "LoggerListener"; }
};

class AuditListener : public EventListener {
public:
    void onEvent(const Event& event) override {
        // In production: write to audit table with full details
        Logger::log(LogLevel::INFO, "AUDIT: " + event.entityType + " operation: " + event.details);
    }
    std::string getName() const override { return "AuditListener"; }
};

class AnalyticsListener : public EventListener {
public:
    void onEvent(const Event& event) override {
        // In production: update metrics, send to analytics service
        Logger::log(LogLevel::DEBUG, "ANALYTICS: Tracked " + event.entityType + " event");
    }
    std::string getName() const override { return "AnalyticsListener"; }
};

// Global listener instances (for demo purposes)
static LoggerListener* loggerListener = nullptr;
static AuditListener* auditListener = nullptr;
static AnalyticsListener* analyticsListener = nullptr;

void initializeEventListeners() {
    loggerListener = new LoggerListener();
    auditListener = new AuditListener();
    analyticsListener = new AnalyticsListener();
    
    EventBus::getInstance().subscribe(loggerListener);
    EventBus::getInstance().subscribe(auditListener);
    EventBus::getInstance().subscribe(analyticsListener);
}

void cleanupEventListeners() {
    if (loggerListener) { EventBus::getInstance().unsubscribe(loggerListener); delete loggerListener; }
    if (auditListener) { EventBus::getInstance().unsubscribe(auditListener); delete auditListener; }
    if (analyticsListener) { EventBus::getInstance().unsubscribe(analyticsListener); delete analyticsListener; }
}
