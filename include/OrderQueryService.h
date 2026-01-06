#ifndef ORDER_QUERY_SERVICE_H
#define ORDER_QUERY_SERVICE_H

#include "Models.h"
#include <string>
#include <vector>

/**
 * @class OrderQueryService
 * @brief CQRS Read Model - All read-only queries
 * 
 * Optimized for reading:
 * - Get order by ID
 * - Get all orders
 * - Get active orders
 * - Get order history
 * - Get orders by customer
 * 
 * No side effects, safe to call from UI.
 */
class OrderQueryService {
public:
    static OrderQueryService& instance();
    
    // Queries (read-only operations, no side effects)
    Order getOrder(const std::string& orderId);
    std::vector<Order> getAllOrders();
    std::vector<Order> getActiveOrders();
    std::vector<Order> getOrdersByCustomer(const std::string& customerId);
    std::vector<Order> getOrdersByStatus(OrderStatus status);
    Order getLastOrder();
    
private:
    OrderQueryService() = default;
};

#endif
