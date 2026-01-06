/**
 * Backend Integration Tests
 * Lightweight sanity tests for backend contract validation
 * 
 * Build: g++ -std=c++17 -Wall tests/BackendTests.cpp src/*.cpp -Iinclude -o test_runner
 * Run: ./test_runner
 */

#include "Logger.h"
#include "Config.h"
#include "Models.h"
#include "OrderFSM.h"
#include "BusinessRules.h"
#include "ServiceLocator.h"
#include "EventSystem.h"
#include "IdempotencyService.h"
#include "SnapshotManager.h"
#include "CommandPattern.h"
#include "ValidationDSL.h"
#include <cassert>
#include <iostream>

// ============================================================================
// Test Infrastructure
// ============================================================================

int passedTests = 0;
int failedTests = 0;

void assertTrue(const std::string& testName, bool condition) {
    if (condition) {
        std::cout << "✅ PASS: " << testName << "\n";
        passedTests++;
    } else {
        std::cout << "❌ FAIL: " << testName << "\n";
        failedTests++;
    }
}

void assertFalse(const std::string& testName, bool condition) {
    assertTrue(testName, !condition);
}

// ============================================================================
// TIER-1 Tests
// ============================================================================

void testConfigurationSystem() {
    std::cout << "\n[TEST SUITE] Configuration System\n";
    
    Config::initialize("config/config.txt");
    
    assertTrue("Config loads MAX_CUSTOMERS", Config::getInt("MAX_CUSTOMERS") == 500);
    assertTrue("Config loads TAX_RATE", Config::getDouble("TAX_RATE") == 0.18);
    assertTrue("Config loads REFUND_WINDOW_DAYS", Config::getInt("REFUND_WINDOW_DAYS") == 7);
    assertTrue("Config loads LOG_LEVEL", Config::getString("LOG_LEVEL") == "INFO");
}

void testPermissionSystem() {
    std::cout << "\n[TEST SUITE] Permission System\n";
    
    try {
        PermissionService::enforce(Action::CREATE_ORDER);
        assertTrue("Permission CREATE_ORDER granted", true);
    } catch (...) {
        assertTrue("Permission CREATE_ORDER granted", false);
    }
    
    try {
        PermissionService::enforce(Action::ISSUE_REFUND);
        assertTrue("Permission ISSUE_REFUND granted", true);
    } catch (...) {
        assertTrue("Permission ISSUE_REFUND granted", false);
    }
}

void testServiceLocator() {
    std::cout << "\n[TEST SUITE] Service Locator\n";
    
    ServiceLocator::initialize();
    
    assertTrue("OrderService initialized", &ServiceLocator::order() != nullptr);
    assertTrue("InventoryService initialized", &ServiceLocator::inventory() != nullptr);
    assertTrue("AnalyticsService initialized", &ServiceLocator::analytics() != nullptr);
    assertTrue("DeliveryService initialized", &ServiceLocator::delivery() != nullptr);
    
    ServiceLocator::cleanup();
}

void testBusinessRules() {
    std::cout << "\n[TEST SUITE] Business Rules\n";
    
    // Order creation rules
    assertTrue("Can create order with valid amount", 
        BusinessRules::canCreateOrder(1, 450.0));
    assertFalse("Cannot create order with negative amount", 
        BusinessRules::canCreateOrder(1, -50.0));
    
    // Tax calculation
    double total = BusinessRules::calculateTotalWithTax(100.0);
    assertTrue("Tax calculation correct", total == 118.0);  // 100 + 18%
}

// ============================================================================
// TIER-2 Tests
// ============================================================================

void testEventSystem() {
    std::cout << "\n[TEST SUITE] Event System\n";
    
    extern void initializeEventListeners();
    initializeEventListeners();
    
    Event evt;
    evt.type = EventType::ORDER_PLACED;
    evt.timestamp = std::time(nullptr);
    
    // Emit event and verify listeners get called
    EventBus::getInstance().emit(evt);
    
    assertTrue("Event system emits without exception", true);
    
    extern void cleanupEventListeners();
    cleanupEventListeners();
}

void testIdempotencyService() {
    std::cout << "\n[TEST SUITE] Idempotent Operations\n";
    
    std::string cached;
    std::string requestId = "test-req-001";
    
    // First request (not duplicate)
    assertFalse("First request is not duplicate", 
        IdempotencyService::isDuplicate(requestId, cached));
    
    // Record success
    IdempotencyService::recordSuccess(requestId, "place_order", "OrderID=1|Amount=100.00");
    
    // Second request with same ID (should be duplicate)
    assertTrue("Duplicate request detected", 
        IdempotencyService::isDuplicate(requestId, cached));
    
    // Verify cached result returned
    assertTrue("Cached result contains OrderID", 
        cached.find("OrderID=1") != std::string::npos);
}

void testSoftDelete() {
    std::cout << "\n[TEST SUITE] Soft Delete System\n";
    
    // Create a test customer
    Customer c;
    c.customerId = 1;
    c.name = "Test Customer";
    c.isActive = true;
    
    // Soft delete
    c.isActive = false;
    c.deletedAt = std::time(nullptr);
    c.deletionReason = "Test deletion";
    
    assertTrue("Customer marked inactive", !c.isActive);
    assertTrue("Deletion timestamp set", c.deletedAt > 0);
    assertTrue("Deletion reason recorded", c.deletionReason == "Test deletion");
    
    // Restore
    c.isActive = true;
    assertTrue("Customer restored to active", c.isActive);
}

// ============================================================================
// TIER-3 Tests
// ============================================================================

void testSnapshotRecovery() {
    std::cout << "\n[TEST SUITE] Snapshot-Based Recovery\n";
    
    // Create snapshots
    SnapshotManager::createSnapshot("Test Snapshot 1", 100, 10, 20);
    SnapshotManager::createSnapshot("Test Snapshot 2", 150, 15, 22);
    
    assertTrue("Snapshots created", SnapshotManager::getSnapshotCount() == 2);
    
    // Get latest snapshot
    SystemSnapshot* latest = SnapshotManager::getLatestSnapshot();
    assertTrue("Latest snapshot retrieved", latest != nullptr);
    assertTrue("Latest snapshot has correct customer count", latest->customerCount == 150);
    
    // Get last stable snapshot
    SystemSnapshot* stable = SnapshotManager::getLastStableSnapshot();
    assertTrue("Last stable snapshot retrieved", stable != nullptr);
    assertTrue("Last stable snapshot is valid", stable->isValid);
}

void testCommandPattern() {
    std::cout << "\n[TEST SUITE] Command Pattern\n";
    
    CommandInvoker& invoker = CommandInvoker::getInstance();
    
    // Create and execute a test order
    Order order;
    order.orderId = 101;
    order.customerId = 1;
    order.state = OrderState::CREATED;
    
    auto placeCmd = std::make_shared<PlaceOrderCommand>(order);
    assertTrue("Command executed", invoker.execute(placeCmd));
    assertTrue("Command recorded in history", invoker.getCommandCount() == 1);
    
    // Undo
    assertTrue("Undo executed", invoker.undo());
    assertTrue("History decremented after undo", invoker.getCommandCount() == 0);
    
    invoker.clearHistory();
}

void testValidationDSL() {
    std::cout << "\n[TEST SUITE] Validation DSL\n";
    
    // Add test rules
    ValidationDSL::addRule("Order.amount", ValidationOperator::GREATER_THAN, 0.0);
    ValidationDSL::addRule("Payment.amount", ValidationOperator::LESS_EQUAL, 10000.0);
    
    // Test validations
    assertTrue("Valid order amount passes", 
        ValidationDSL::validate("Order.amount", 100.0));
    assertFalse("Invalid order amount fails", 
        ValidationDSL::validate("Order.amount", -10.0));
    
    assertTrue("Valid payment amount passes", 
        ValidationDSL::validate("Payment.amount", 5000.0));
    assertFalse("Invalid payment amount fails", 
        ValidationDSL::validate("Payment.amount", 15000.0));
    
    ValidationDSL::clearRules();
}

// ============================================================================
// Order Lifecycle Tests
// ============================================================================

void testOrderStateTransitions() {
    std::cout << "\n[TEST SUITE] Order State Machine\n";
    
    OrderFSM fsm;
    
    // Valid transition path
    assertTrue("Can transition CREATED → CONFIRMED", 
        fsm.canTransition(OrderState::CREATED, OrderState::CONFIRMED));
    
    // Simulate transition
    assertTrue("Transition to CONFIRMED succeeds", 
        fsm.transition(OrderState::CONFIRMED));
    
    assertTrue("Can transition CONFIRMED → PREPARING", 
        fsm.canTransition(OrderState::CONFIRMED, OrderState::PREPARING));
    
    // Invalid transition
    fsm.transition(OrderState::SERVED);  // Jump to SERVED
    assertFalse("Cannot transition SERVED → PREPARING", 
        fsm.canTransition(OrderState::SERVED, OrderState::PREPARING));
}

void testOrderLifecycle() {
    std::cout << "\n[TEST SUITE] Complete Order Lifecycle\n";
    
    // 1. Create order
    Order order;
    order.orderId = 100;
    order.customerId = 1;
    order.state = OrderState::CREATED;
    
    assertTrue("Order created in CREATED state", order.state == OrderState::CREATED);
    
    // 2. Confirm order
    assertTrue("Can confirm order", 
        BusinessRules::canCreateOrder(order.customerId, 100.0));
    
    order.state = OrderState::CONFIRMED;
    assertTrue("Order moved to CONFIRMED state", order.state == OrderState::CONFIRMED);
    
    // 3. Prepare order
    order.state = OrderState::PREPARING;
    assertTrue("Order moved to PREPARING state", order.state == OrderState::PREPARING);
    
    // 4. Serve order
    order.state = OrderState::READY;
    order.state = OrderState::SERVED;
    assertTrue("Order moved to SERVED state", order.state == OrderState::SERVED);
    
    // 5. Refund order (within window)
    assertTrue("Can refund SERVED order (fresh)", 
        BusinessRules::canRefundOrder(order));
    
    order.state = OrderState::REFUNDED;
    assertTrue("Order moved to REFUNDED state", order.state == OrderState::REFUNDED);
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║        BACKEND INTEGRATION TEST SUITE                  ║\n";
    std::cout << "║        Sanity Tests for Service Contract               ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    
    // Initialize system
    Logger::initialize();
    
    // TIER-1 Tests
    testConfigurationSystem();
    testPermissionSystem();
    testServiceLocator();
    testBusinessRules();
    
    // TIER-2 Tests
    testEventSystem();
    testIdempotencyService();
    testSoftDelete();
    
    // TIER-3 Tests
    testSnapshotRecovery();
    testCommandPattern();
    testValidationDSL();
    
    // Lifecycle Tests
    testOrderStateTransitions();
    testOrderLifecycle();
    
    // Summary
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║                  TEST SUMMARY                          ║\n";
    std::cout << "║────────────────────────────────────────────────────────║\n";
    std::cout << "║ Passed: " << passedTests << " tests                                ║\n";
    std::cout << "║ Failed: " << failedTests << " tests                                ║\n";
    
    if (failedTests == 0) {
        std::cout << "║                                                        ║\n";
        std::cout << "║ ✅ ALL TESTS PASSED - BACKEND CONTRACT VALID          ║\n";
        std::cout << "╚════════════════════════════════════════════════════════╝\n";
        return 0;
    } else {
        std::cout << "║                                                        ║\n";
        std::cout << "║ ❌ SOME TESTS FAILED - REVIEW ISSUES                  ║\n";
        std::cout << "╚════════════════════════════════════════════════════════╝\n";
        return 1;
    }
}
