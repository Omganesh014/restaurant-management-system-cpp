#pragma once
#include "Models.h"
#include <string>

/**
 * Business Rule Engine
 * Centralizes all business logic validation rules
 * Makes rules explicit, testable, and auditable
 * Separates concerns: "Can I do X?" vs "How do I do X?"
 */
class BusinessRules {
public:
    // Order rules
    static bool canCreateOrder(int customerId, double amount);
    static bool canModifyOrder(const Order& order);
    static bool canCancelOrder(const Order& order);
    static bool canRefundOrder(const Order& order);
    static bool canServeOrder(const Order& order);
    
    // Customer rules
    static bool canApplyLoyaltyPoints(int customerId, int points);
    static bool isEligibleForDiscount(int customerId);
    
    // Inventory rules
    static bool canReduceInventory(const std::string& itemName, int quantity);
    static bool needsReorder(const std::string& itemName, int currentQuantity, int reorderLevel);
    
    // Payment rules
    static bool isValidPaymentAmount(double amount);
    static double calculateTotalWithTax(double subtotal);
    
    // Refund rules
    static bool isWithinRefundWindow(const Order& order);
    static double calculateRefundAmount(const Order& order);
    
    // Validation helpers
    static std::string getViolationMessage();

private:
    static std::string lastViolationMessage;
};
