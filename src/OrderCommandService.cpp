#include "OrderCommandService.h"
#include "ServiceLocator.h"
#include "EventSystem.h"
#include "BusinessRules.h"
#include "Logger.h"

OrderCommandService& OrderCommandService::instance() {
    static OrderCommandService ocs;
    return ocs;
}

Order OrderCommandService::createOrder(const std::string& customerId, 
                                       const std::vector<MenuItem>& items,
                                       const std::string& requestId) {
    Logger::log("COMMAND: Creating order for customer " + customerId);
    
    Order order;
    order.id = "ORD-" + std::to_string(std::time(nullptr));
    order.customerId = customerId;
    order.items = items;
    order.status = OrderStatus::CREATED;
    order.createdAt = std::time(nullptr);
    
    double subtotal = 0.0;
    for (const auto& item : items) {
        subtotal += item.price;
    }
    order.subtotal = subtotal;
    order.tax = subtotal * 0.08;
    order.total = order.subtotal + order.tax;
    
    Logger::log("COMMAND: Order created with ID " + order.id);
    
    // Emit event
    OrderCreatedEvent event;
    event.orderId = order.id;
    event.total = order.total;
    ServiceLocator::instance().getEventSystem().publish(event);
    
    return order;
}

void OrderCommandService::confirmOrder(const std::string& orderId) {
    Logger::log("COMMAND: Confirming order " + orderId);
    
    // In a real system, verify payment, update inventory, etc.
    // For this demo, just log the command
    
    OrderConfirmedEvent event;
    event.orderId = orderId;
    ServiceLocator::instance().getEventSystem().publish(event);
}

void OrderCommandService::markAsServing(const std::string& orderId) {
    Logger::log("COMMAND: Marking order " + orderId + " as SERVING");
    
    OrderServingEvent event;
    event.orderId = orderId;
    ServiceLocator::instance().getEventSystem().publish(event);
}

void OrderCommandService::markAsReady(const std::string& orderId) {
    Logger::log("COMMAND: Marking order " + orderId + " as READY");
    
    // No specific event for ready, but tracked in OrderReadyEvent
}

void OrderCommandService::markAsServed(const std::string& orderId) {
    Logger::log("COMMAND: Marking order " + orderId + " as SERVED");
    
    OrderServedEvent event;
    event.orderId = orderId;
    ServiceLocator::instance().getEventSystem().publish(event);
}

void OrderCommandService::cancelOrder(const std::string& orderId) {
    Logger::log("COMMAND: Cancelling order " + orderId);
    
    OrderCancelledEvent event;
    event.orderId = orderId;
    ServiceLocator::instance().getEventSystem().publish(event);
}

void OrderCommandService::issueRefund(const std::string& orderId, const std::string& reason) {
    Logger::log("COMMAND: Issuing refund for order " + orderId + " - Reason: " + reason);
    
    RefundProcessedEvent event;
    event.orderId = orderId;
    event.reason = reason;
    ServiceLocator::instance().getEventSystem().publish(event);
}
