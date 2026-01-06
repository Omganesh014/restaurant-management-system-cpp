#include "OrderQueryService.h"
#include "Logger.h"
#include <algorithm>
#include <fstream>
#include <sstream>

OrderQueryService& OrderQueryService::instance() {
    static OrderQueryService oqs;
    return oqs;
}

Order OrderQueryService::getOrder(const std::string& orderId) {
    Logger::log("QUERY: Getting order " + orderId);
    
    // Simulate querying (in real system, read from optimized index)
    Order order;
    order.id = orderId;
    return order;
}

std::vector<Order> OrderQueryService::getAllOrders() {
    Logger::log("QUERY: Getting all orders");
    
    // Simulate reading all orders (in real system, read from indexed view)
    std::vector<Order> orders;
    
    try {
        std::ifstream file("data/orders.txt");
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                if (!line.empty()) {
                    Order order;
                    order.id = line;
                    orders.push_back(order);
                }
            }
            file.close();
        }
    } catch (...) {
        Logger::log("Error reading orders");
    }
    
    return orders;
}

std::vector<Order> OrderQueryService::getActiveOrders() {
    Logger::log("QUERY: Getting active orders");
    
    // In real CQRS, this would hit a materialized view
    // Active = not SERVED, not REFUNDED, not CANCELLED
    std::vector<Order> active;
    auto all = getAllOrders();
    
    for (const auto& order : all) {
        if (order.status != OrderStatus::SERVED && 
            order.status != OrderStatus::REFUNDED && 
            order.status != OrderStatus::CANCELLED) {
            active.push_back(order);
        }
    }
    
    return active;
}

std::vector<Order> OrderQueryService::getOrdersByCustomer(const std::string& customerId) {
    Logger::log("QUERY: Getting orders for customer " + customerId);
    
    std::vector<Order> customerOrders;
    auto all = getAllOrders();
    
    for (const auto& order : all) {
        if (order.customerId == customerId) {
            customerOrders.push_back(order);
        }
    }
    
    return customerOrders;
}

std::vector<Order> OrderQueryService::getOrdersByStatus(OrderStatus status) {
    Logger::log("QUERY: Getting orders by status");
    
    std::vector<Order> statusOrders;
    auto all = getAllOrders();
    
    for (const auto& order : all) {
        if (order.status == status) {
            statusOrders.push_back(order);
        }
    }
    
    return statusOrders;
}

Order OrderQueryService::getLastOrder() {
    Logger::log("QUERY: Getting last order");
    
    auto orders = getAllOrders();
    if (orders.empty()) {
        return Order();
    }
    return orders.back();
}
