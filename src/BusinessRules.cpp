#include "BusinessRules.h"
#include "Config.h"
#include "Logger.h"
#include <ctime>

std::string BusinessRules::lastViolationMessage;

// ============================================================================
// ORDER RULES
// ============================================================================

bool BusinessRules::canCreateOrder(int customerId, double amount) {
    if (customerId <= 0) {
        lastViolationMessage = "Invalid customer ID";
        return false;
    }
    if (amount <= 0) {
        lastViolationMessage = "Order amount must be positive";
        return false;
    }
    return true;
}

bool BusinessRules::canModifyOrder(const Order& order) {
    // Can only modify if order is in CREATED or CONFIRMED state
    if (order.state != OrderState::CREATED && order.state != OrderState::CONFIRMED) {
        lastViolationMessage = "Cannot modify order in " + OrderFSM::toString(order.state) + " state";
        return false;
    }
    return true;
}

bool BusinessRules::canCancelOrder(const Order& order) {
    // Cannot cancel if already served or refunded
    if (order.state == OrderState::SERVED || order.state == OrderState::REFUNDED || order.state == OrderState::CANCELLED) {
        lastViolationMessage = "Cannot cancel order in " + OrderFSM::toString(order.state) + " state";
        return false;
    }
    return true;
}

bool BusinessRules::canRefundOrder(const Order& order) {
    if (order.state != OrderState::SERVED) {
        lastViolationMessage = "Can only refund SERVED orders";
        return false;
    }
    if (!isWithinRefundWindow(order)) {
        lastViolationMessage = "Order is outside refund window";
        return false;
    }
    return true;
}

bool BusinessRules::canServeOrder(const Order& order) {
    if (order.state != OrderState::READY) {
        lastViolationMessage = "Order must be READY before serving";
        return false;
    }
    return true;
}

// ============================================================================
// CUSTOMER RULES
// ============================================================================

bool BusinessRules::canApplyLoyaltyPoints(int customerId, int points) {
    if (customerId <= 0) {
        lastViolationMessage = "Invalid customer ID";
        return false;
    }
    if (points < 0) {
        lastViolationMessage = "Loyalty points cannot be negative";
        return false;
    }
    return true;
}

bool BusinessRules::isEligibleForDiscount(int customerId) {
    // Placeholder: expand with loyalty tier logic
    return customerId > 0;
}

// ============================================================================
// INVENTORY RULES
// ============================================================================

bool BusinessRules::canReduceInventory(const std::string& itemName, int quantity) {
    if (itemName.empty()) {
        lastViolationMessage = "Item name cannot be empty";
        return false;
    }
    if (quantity <= 0) {
        lastViolationMessage = "Quantity must be positive";
        return false;
    }
    return true;
}

bool BusinessRules::needsReorder(const std::string& itemName, int currentQuantity, int reorderLevel) {
    return currentQuantity <= reorderLevel;
}

// ============================================================================
// PAYMENT RULES
// ============================================================================

bool BusinessRules::isValidPaymentAmount(double amount) {
    if (amount <= 0) {
        lastViolationMessage = "Payment amount must be positive";
        return false;
    }
    if (amount > 1000000) {
        lastViolationMessage = "Payment amount exceeds maximum limit";
        return false;
    }
    return true;
}

double BusinessRules::calculateTotalWithTax(double subtotal) {
    double taxRate = Config::getDouble("TAX_RATE", 0.18);
    return subtotal * (1.0 + taxRate);
}

// ============================================================================
// REFUND RULES
// ============================================================================

bool BusinessRules::isWithinRefundWindow(const Order& order) {
    int refundWindowDays = Config::getInt("REFUND_WINDOW_DAYS", 7);
    std::time_t now = std::time(nullptr);
    std::time_t orderAge = now - order.timestamp;
    int daysSinceOrder = orderAge / (24 * 3600);
    return daysSinceOrder <= refundWindowDays;
}

double BusinessRules::calculateRefundAmount(const Order& order) {
    // Full refund if within window
    if (isWithinRefundWindow(order)) {
        return order.total;
    }
    // Partial refund (50%) if outside window
    return order.total * 0.5;
}

std::string BusinessRules::getViolationMessage() {
    return lastViolationMessage;
}
