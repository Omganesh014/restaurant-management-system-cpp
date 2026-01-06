#ifndef ORDER_COMMAND_SERVICE_H
#define ORDER_COMMAND_SERVICE_H

#include "Models.h"
#include <string>
#include <vector>

/**
 * @class OrderCommandService
 * @brief CQRS Write Model - All state-changing operations
 * 
 * Handles:
 * - Create order
 * - Confirm order
 * - Cancel order
 * - Issue refund
 * - Mark as served
 * 
 * Enforces business rules and maintains consistency.
 */
class OrderCommandService {
public:
    static OrderCommandService& instance();
    
    // Commands (state-changing operations)
    Order createOrder(const std::string& customerId, 
                      const std::vector<MenuItem>& items,
                      const std::string& requestId = "");
    
    void confirmOrder(const std::string& orderId);
    void markAsServing(const std::string& orderId);
    void markAsReady(const std::string& orderId);
    void markAsServed(const std::string& orderId);
    void cancelOrder(const std::string& orderId);
    void issueRefund(const std::string& orderId, const std::string& reason);
    
private:
    OrderCommandService() = default;
};

#endif
