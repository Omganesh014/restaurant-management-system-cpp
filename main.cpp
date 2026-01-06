#include "Logger.h"
#include "Models.h"
#include "Config.h"
#include "PermissionService.h"
#include "ServiceLocator.h"
#include "BusinessRules.h"
#include "EventSystem.h"
#include "SoftDelete.h"
#include "IdempotencyService.h"
#include "SnapshotManager.h"
#include "CommandPattern.h"
#include "ValidationDSL.h"
#include "TransactionManager.h"
#include "HealthService.h"
#include "OrderCommandService.h"
#include "OrderQueryService.h"
#include "StorageStrategy.h"
#include <iostream>
#include <iomanip>

int main() {
    // ========================================================================
    // SYSTEM INITIALIZATION
    // ========================================================================
    Logger::initialize();
    Logger::log(LogLevel::INFO, "=== RESTAURANT MANAGEMENT SYSTEM v2.1 (Multi-File) ===");
    
    // Load configuration from file
    Config::initialize("config/config.txt");
    Config::logConfiguration();
    
    // Initialize service registry
    ServiceLocator::initialize();
    
    // Initialize event system with listeners
    extern void initializeEventListeners();
    initializeEventListeners();
    
    // ========================================================================
    // DEMONSTRATION: CONFIGURATION SYSTEM
    // ========================================================================
    std::cout << "\n[DEMO] Configuration System\n";
    std::cout << "────────────────────────────────────────────────\n";
    std::cout << "MAX_CUSTOMERS: " << Config::getInt("MAX_CUSTOMERS") << "\n";
    std::cout << "TAX_RATE: " << std::fixed << std::setprecision(2) 
              << Config::getDouble("TAX_RATE") * 100 << "%\n";
    std::cout << "LOG_LEVEL: " << Config::getString("LOG_LEVEL") << "\n";
    std::cout << "REFUND_WINDOW_DAYS: " << Config::getInt("REFUND_WINDOW_DAYS") << "\n";
    
    // ========================================================================
    // DEMONSTRATION: PERMISSION HOOKS
    // ========================================================================
    std::cout << "\n[DEMO] Permission System (Role-Agnostic)\n";
    std::cout << "────────────────────────────────────────────────\n";
    PermissionService::enforce(Action::CREATE_ORDER);
    std::cout << "✓ CREATE_ORDER permission granted\n";
    
    PermissionService::enforce(Action::ISSUE_REFUND);
    std::cout << "✓ ISSUE_REFUND permission granted\n";
    
    PermissionService::enforce(Action::MANAGE_INVENTORY);
    std::cout << "✓ MANAGE_INVENTORY permission granted\n";
    
    // ========================================================================
    // DEMONSTRATION: BUSINESS RULES ENGINE
    // ========================================================================
    std::cout << "\n[DEMO] Business Rules Engine\n";
    std::cout << "────────────────────────────────────────────────\n";
    
    // Test Order Creation Rule
    bool canCreate = BusinessRules::canCreateOrder(101, 450.0);
    std::cout << "Can create order for customer 101, amount 450.0? " 
              << (canCreate ? "YES" : "NO") << "\n";
    
    // Test Order Modification Rule
    Order testOrder {1, 101, 450.0, 5, time(nullptr), OrderState::CREATED};
    bool canModify = BusinessRules::canModifyOrder(testOrder);
    std::cout << "Can modify order in CREATED state? " 
              << (canModify ? "YES" : "NO") << "\n";
    
    testOrder.state = OrderState::SERVED;
    canModify = BusinessRules::canModifyOrder(testOrder);
    std::cout << "Can modify order in SERVED state? " 
              << (canModify ? "YES" : "NO") << "\n";
    
    // Test Refund Rule
    testOrder.state = OrderState::SERVED;
    testOrder.timestamp = time(nullptr); // Fresh timestamp
    bool canRefund = BusinessRules::canRefundOrder(testOrder);
    std::cout << "Can refund SERVED order (fresh)? " 
              << (canRefund ? "YES" : "NO") << "\n";
    
    // Test Tax Calculation
    double subtotal = 100.0;
    double withTax = BusinessRules::calculateTotalWithTax(subtotal);
    std::cout << "Subtotal $" << subtotal << " + Tax = $" 
              << std::fixed << std::setprecision(2) << withTax << "\n";
    
    // ========================================================================
    // DEMONSTRATION: FSM WITH BUSINESS RULES
    // ========================================================================
    std::cout << "\n[DEMO] Order FSM + Business Rules Integration\n";
    std::cout << "────────────────────────────────────────────────\n";
    
    Order order {1, 101, 450.0, 5, time(nullptr), OrderState::CREATED};
    std::cout << "Order #" << order.orderId << " created in state: " 
              << OrderFSM::toString(order.state) << "\n";
    
    // Try valid transitions
    if (order.updateState(OrderState::CONFIRMED)) {
        std::cout << "✓ Transitioned to: " << OrderFSM::toString(order.state) << "\n";
    }
    
    if (order.updateState(OrderState::PREPARING)) {
        std::cout << "✓ Transitioned to: " << OrderFSM::toString(order.state) << "\n";
    }
    
    if (order.updateState(OrderState::READY)) {
        std::cout << "✓ Transitioned to: " << OrderFSM::toString(order.state) << "\n";
    }
    
    if (order.updateState(OrderState::SERVED)) {
        std::cout << "✓ Transitioned to: " << OrderFSM::toString(order.state) << "\n";
    }
    
    // Try invalid transition (should fail)
    if (!order.updateState(OrderState::PREPARING)) {
        std::cout << "✗ Invalid transition from SERVED to PREPARING (blocked)\n";
    }
    
    // ========================================================================
    // DEMONSTRATION: EVENT SYSTEM (OBSERVER PATTERN)
    // ========================================================================
    std::cout << "\n[DEMO] Event System (Observer Pattern)\n";
    std::cout << "────────────────────────────────────────────────\n";
    
    EventBus& eventBus = EventBus::getInstance();
    
    std::cout << "Emitting ORDER_PLACED event...\n";
    Event e1 {EventType::ORDER_PLACED, 1, "Order", "Customer #101 placed order $450.00", time(nullptr), "OrderService"};
    eventBus.emit(e1);
    
    std::cout << "\nEmitting ORDER_CONFIRMED event...\n";
    Event e2 {EventType::ORDER_CONFIRMED, 1, "Order", "Order confirmed by staff", time(nullptr), "OrderService"};
    eventBus.emit(e2);
    
    std::cout << "\nEmitting INVENTORY_LOW event...\n";
    Event e3 {EventType::INVENTORY_LOW, 42, "Inventory", "Rice inventory below reorder level", time(nullptr), "InventoryService"};
    eventBus.emit(e3);
    
    std::cout << "\nEmitting ORDER_SERVED event...\n";
    Event e4 {EventType::ORDER_SERVED, 1, "Order", "Order ready and served to customer", time(nullptr), "OrderService"};
    eventBus.emit(e4);
    
    // ========================================================================
    // DEMONSTRATION: SOFT DELETE SYSTEM
    // ========================================================================
    std::cout << "\n[DEMO] Soft Delete System (Data Retention)\n";
    std::cout << "────────────────────────────────────────────────\n";
    
    // Create a customer record with soft delete fields
    CustomerRecord customer {101, "Alice Johnson", "555-1234", "alice@example.com", 500};
    std::cout << "Created Customer #" << customer.id << " (" << customer.name << ")\n";
    std::cout << "Active: " << (customer.isActive ? "YES" : "NO") << "\n";
    
    // Soft delete the customer (don't actually remove data)
    customer.softDelete("Customer requested account closure");
    std::cout << "Soft deleted: " << customer.name << "\n";
    std::cout << "Active: " << (customer.isActive ? "YES" : "NO") << "\n";
    std::cout << "Deletion reason: " << customer.deletionReason << "\n";
    std::cout << "Data preserved for: audit, history, recovery\n";
    
    // Show restore capability
    std::cout << "\nRestoring customer...\n";
    customer.restore();
    std::cout << "Active: " << (customer.isActive ? "YES" : "NO") << "\n";
    
    // Menu item soft delete
    std::cout << "\nSoft deleting menu item...\n";
    MenuItemRecord menuItem {10, "Spicy Pasta", "Main Course", 12.99};
    menuItem.softDelete("Item temporarily out of stock");
    std::cout << "Menu item '" << menuItem.name << "' marked inactive\n";
    std::cout << "Reason: " << menuItem.deletionReason << "\n";
    std::cout << "Old queries skip deleted items automatically\n";
    
    // ========================================================================
    // DEMONSTRATION: IDEMPOTENT OPERATIONS
    // ========================================================================
    std::cout << "\n[DEMO] Idempotent Operations (Prevent Duplicates)\n";
    std::cout << "────────────────────────────────────────────────\n";
    
    IdempotencyService::setDefaultTTL(3600);  // 1 hour
    
    std::string requestId1 = "req-2024-001";
    std::string requestId2 = "req-2024-002";
    
    // First request: place order
    std::cout << "Request 1: Placing order with requestId=" << requestId1 << "\n";
    std::string cached;
    if (!IdempotencyService::isDuplicate(requestId1, cached)) {
        std::cout << "  → New request, processing...\n";
        IdempotencyService::recordSuccess(requestId1, "place_order", "OrderID=1|Amount=450.00");
        std::cout << "  → Order placed successfully\n";
    }
    
    // Second request: same requestId (network retry scenario)
    std::cout << "\nRequest 2: Retry same requestId=" << requestId1 << "\n";
    if (IdempotencyService::isDuplicate(requestId1, cached)) {
        std::cout << "  → Duplicate detected! Returning cached: " << cached << "\n";
        std::cout << "  → No double-charge, no duplicate order\n";
    }
    
    // Third request: different requestId
    std::cout << "\nRequest 3: Different requestId=" << requestId2 << "\n";
    if (!IdempotencyService::isDuplicate(requestId2, cached)) {
        std::cout << "  → New request, processing...\n";
        IdempotencyService::recordSuccess(requestId2, "process_payment", "TxnID=TXN-789");
    }
    
    std::cout << "\nIdempotency status: " << IdempotencyService::getTrackedCount() 
              << " request(s) tracked\n";
    
    // ========================================================================
    // DEMONSTRATION: SNAPSHOT-BASED RECOVERY (TIER-3 Feature #8)
    // ========================================================================
    std::cout << "\n[DEMO] Snapshot-Based Recovery System\n";
    std::cout << "────────────────────────────────────────────────\n";
    
    // Create first snapshot (morning state)
    SnapshotManager::createSnapshot("Morning initialization", 150, 5, 20);
    SnapshotManager::createSnapshot("Mid-morning activity", 150, 8, 20);
    SnapshotManager::createSnapshot("End of shift backup", 152, 25, 22);
    
    std::cout << "\nTotal snapshots: " << SnapshotManager::getSnapshotCount() << "\n";
    
    std::cout << "Snapshot History:\n";
    for (const auto& snap : SnapshotManager::listSnapshots()) {
        std::cout << "  ID: " << snap.snapshotId << " | " << snap.description << "\n";
        std::cout << "     Status: " << (snap.isValid ? "VALID" : "INVALID") << "\n";
        std::cout << "     Entities - C:" << snap.customerCount << " O:" << snap.orderCount << " M:" << snap.menuItemCount << "\n";
    }
    
    // Recovery
    SystemSnapshot* lastStable = SnapshotManager::getLastStableSnapshot();
    if (lastStable) {
        std::cout << "\nRecovering from: " << lastStable->snapshotId << "\n";
        SnapshotManager::restoreFromSnapshot(lastStable->snapshotId);
    }
    
    // ========================================================================
    // DEMONSTRATION: COMMAND PATTERN (TIER-3 Feature #9)
    // ========================================================================
    std::cout << "\n[DEMO] Command Pattern with Undo/Replay\n";
    std::cout << "────────────────────────────────────────────────\n";
    
    CommandInvoker& invoker = CommandInvoker::getInstance();
    
    // Create and execute orders via commands
    Order order1;
    order1.orderId = 101;
    order1.customerId = 1;
    order1.state = OrderState::CREATED;
    
    std::cout << "Executing PlaceOrderCommand...\n";
    auto placeCmd = std::make_shared<PlaceOrderCommand>(order1);
    invoker.execute(placeCmd);
    
    std::cout << "Executing ConfirmOrderCommand...\n";
    auto confirmCmd = std::make_shared<ConfirmOrderCommand>(101);
    invoker.execute(confirmCmd);
    
    std::cout << "Executing IssueRefundCommand...\n";
    auto refundCmd = std::make_shared<IssueRefundCommand>(101, 45.99, "Customer Request");
    invoker.execute(refundCmd);
    
    std::cout << "\nCommand History (Total: " << invoker.getCommandCount() << ")\n";
    int idx = 1;
    for (const auto& cmd : invoker.getHistory()) {
        std::cout << "  [" << (idx++) << "] " << cmd->getName() << ": " << cmd->getDescription() << "\n";
    }
    
    std::cout << "\nReplaying Command Sequence:\n";
    invoker.replayCommands();
    
    std::cout << "\nUndoing last command...\n";
    invoker.undo();
    std::cout << "Remaining commands: " << invoker.getCommandCount() << "\n";
    
    // ========================================================================
    // DEMONSTRATION: VALIDATION DSL (TIER-3 Feature #10)
    // ========================================================================
    std::cout << "\n[DEMO] Validation DSL (Runtime Rule Validation)\n";
    std::cout << "────────────────────────────────────────────────\n";
    
    // Add validation rules programmatically
    ValidationDSL::addRule("Order.amount", ValidationOperator::GREATER_THAN, 0.0, "Orders must have positive amount");
    ValidationDSL::addRule("Inventory.quantity", ValidationOperator::GREATER_EQUAL, 0.0, "Inventory can't be negative");
    ValidationDSL::addRule("Customer.age", ValidationOperator::GREATER_EQUAL, 18.0, "Customer must be adult");
    ValidationDSL::addRule("Payment.amount", ValidationOperator::LESS_EQUAL, 10000.0, "Single payment limit");
    
    std::cout << "Registered " << ValidationDSL::getRules().size() << " validation rules\n\n";
    
    // Test validations
    std::cout << "Testing Order.amount validation:\n";
    bool valid1 = ValidationDSL::validate("Order.amount", 45.99);
    std::cout << "  Order amount $45.99: " << (valid1 ? "VALID" : "INVALID") << "\n";
    
    bool valid2 = ValidationDSL::validate("Order.amount", -10.0);
    std::cout << "  Order amount $-10.0: " << (valid2 ? "VALID" : "INVALID") << "\n";
    
    std::cout << "\nTesting Inventory validation:\n";
    bool valid3 = ValidationDSL::validate("Inventory.quantity", 50.0);
    std::cout << "  Inventory quantity 50 units: " << (valid3 ? "VALID" : "INVALID") << "\n";
    
    bool valid4 = ValidationDSL::validate("Inventory.quantity", -5.0);
    std::cout << "  Inventory quantity -5 units: " << (valid4 ? "VALID" : "INVALID") << "\n";
    
    std::cout << "\nTesting Payment validation:\n";
    bool valid5 = ValidationDSL::validate("Payment.amount", 500.0);
    std::cout << "  Payment amount $500: " << (valid5 ? "VALID" : "INVALID") << "\n";
    
    bool valid6 = ValidationDSL::validate("Payment.amount", 15000.0);
    std::cout << "  Payment amount $15000: " << (valid6 ? "VALID" : "INVALID") << "\n";
    
    // Test category validation
    std::cout << "\nTesting Category Validation (Order rules):\n";
    std::map<std::string, double> orderData;
    orderData["Order.amount"] = 99.99;
    bool categoryValid = ValidationDSL::validateCategory("Order", orderData);
    std::cout << "  Order validation result: " << (categoryValid ? "PASSED" : "FAILED") << "\n";
    
    // ========================================================================
    // SYSTEM SHUTDOWN
    // ========================================================================
    std::cout << "\n[INFO] Performing graceful shutdown...\n";
    extern void cleanupEventListeners();
    cleanupEventListeners();
    ServiceLocator::cleanup();
    
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║  TIER-1 Features Initialized Successfully              ║\n";
    std::cout << "║  ✓ Configuration System (Externalized Config)           ║\n";
    std::cout << "║  ✓ Permission Hooks (Role-Agnostic RBAC)               ║\n";
    std::cout << "║  ✓ Service Locator (Dependency Injection)              ║\n";
    std::cout << "║  ✓ Business Rules (Centralized Validation)             ║\n";
    std::cout << "╠════════════════════════════════════════════════════════╣\n";
    std::cout << "║  TIER-2 Features Initialized Successfully              ║\n";
    std::cout << "║  ✓ Event System (Observer Pattern)                     ║\n";
    std::cout << "║  ✓ Soft Delete System (Data Retention)                 ║\n";
    std::cout << "║  ✓ Idempotent Operations (Duplicate Prevention)         ║\n";
    std::cout << "╠════════════════════════════════════════════════════════╣\n";
    std::cout << "║  TIER-3 Features Initialized Successfully              ║\n";
    std::cout << "║  ✓ Snapshot-Based Recovery (System State Snapshots)     ║\n";
    std::cout << "║  ✓ Command Pattern (Undo/Replay Capability)            ║\n";
    std::cout << "║  ✓ Validation DSL (Runtime Rule Validation)            ║\n";
    std::cout << "╠════════════════════════════════════════════════════════╣\n";
    std::cout << "║  TIER-4 Features Initialized Successfully (ELITE)       ║\n";
    std::cout << "║  ✓ Transaction Manager (ACID-like Behavior)            ║\n";
    std::cout << "║  ✓ Health Monitor (System Diagnostics)                 ║\n";
    std::cout << "║  ✓ Read/Write Separation (CQRS-lite)                   ║\n";
    std::cout << "║  ✓ Storage Strategy (Pluggable Architecture)           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";

    // ========================================================================
    // DEMONSTRATION: TRANSACTION MANAGER (TIER-4 Feature #1)
    // ========================================================================
    std::cout << "\n[DEMO] Transaction Manager - ACID-like Behavior\n";
    std::cout << "────────────────────────────────────────────────\n";
    {
        Transaction* tx = TransactionManager::instance().createTransaction();
        tx->begin();
        
        // Register multi-step operation
        bool success = true;
        tx->execute([&](){ std::cout << "  → Step 1: Create order\n"; },
                    [&](){ std::cout << "  ← Rollback: Order creation reversed\n"; });
        
        tx->execute([&](){ std::cout << "  → Step 2: Deduct inventory\n"; },
                    [&](){ std::cout << "  ← Rollback: Inventory restored\n"; });
        
        tx->execute([&](){ std::cout << "  → Step 3: Generate bill\n"; },
                    [&](){ std::cout << "  ← Rollback: Bill reversed\n"; });
        
        tx->commit();
        
        std::cout << "  ✓ Transaction State: " << tx->getStateString() << "\n";
        std::cout << "  ✓ Operations: " << tx->getOperationCount() << " completed\n";
        std::cout << "  ✓ Transaction successful: " << (tx->isSuccessful() ? "YES" : "NO") << "\n";
        
        TransactionManager::instance().removeTransaction(tx);
    }
    std::cout << "  ✓ Active Transactions: " << TransactionManager::instance().getActiveTransactionCount() << "\n";
    std::cout << "  ✓ Total Committed: " << TransactionManager::instance().getTotalCommittedTransactions() << "\n";

    // ========================================================================
    // DEMONSTRATION: HEALTH MONITOR (TIER-4 Feature #2)
    // ========================================================================
    std::cout << "\n[DEMO] Health Monitor - System Diagnostics\n";
    std::cout << "────────────────────────────────────────────────\n";
    {
        auto& healthService = HealthService::instance();
        auto health = healthService.checkHealth();
        
        std::cout << "  Status: " << health.statusString << "\n";
        std::cout << "  CSV Accessible: " << (health.csvAccessible ? "✓" : "✗") << "\n";
        std::cout << "  Snapshots OK: " << (health.snapshotAccessible ? "✓" : "✗") << "\n";
        std::cout << "  Event System: " << (health.eventSystemOperational ? "✓" : "✗") << "\n";
        std::cout << "  Services Ready: " << (health.servicesInitialized ? "✓" : "✗") << "\n";
        std::cout << "  Estimated Memory: " << health.estimatedMemoryMB << " MB\n";
        std::cout << "  Snapshots Available: " << health.snapshotCount << "\n";
        
        // Show detailed report
        std::cout << healthService.getDetailedReport();
    }

    // ========================================================================
    // DEMONSTRATION: READ/WRITE SEPARATION (TIER-4 Feature #3)
    // ========================================================================
    std::cout << "\n[DEMO] Read/Write Separation - CQRS Pattern\n";
    std::cout << "────────────────────────────────────────────────\n";
    {
        auto& commandSvc = OrderCommandService::instance();
        auto& querySvc = OrderQueryService::instance();
        
        // WRITE: Create order via command service
        std::cout << "  WRITE: Creating order via command service...\n";
        std::vector<MenuItem> items;
        MenuItem item1;
        item1.id = "BURGER";
        item1.name = "Burger";
        item1.price = 12.99;
        items.push_back(item1);
        
        Order newOrder = commandSvc.createOrder("CUST-001", items, "REQ-12345");
        std::cout << "  ✓ Order created: " << newOrder.id << " (Total: $" 
                  << std::fixed << std::setprecision(2) << newOrder.total << ")\n";
        
        // WRITE: Confirm order via command service
        std::cout << "  WRITE: Confirming order...\n";
        commandSvc.confirmOrder(newOrder.id);
        std::cout << "  ✓ Order confirmed\n";
        
        // READ: Query orders via query service (no side effects)
        std::cout << "  READ: Querying active orders...\n";
        auto activeOrders = querySvc.getActiveOrders();
        std::cout << "  ✓ Active orders count: " << activeOrders.size() << "\n";
        
        std::cout << "  READ: Querying customer orders...\n";
        auto customerOrders = querySvc.getOrdersByCustomer("CUST-001");
        std::cout << "  ✓ Customer orders: " << customerOrders.size() << "\n";
        
        std::cout << "  READ: Getting last order...\n";
        Order lastOrder = querySvc.getLastOrder();
        std::cout << "  ✓ Last order: " << lastOrder.id << "\n";
    }
    std::cout << "  🎯 CQRS Benefits:\n";
    std::cout << "     - Writes optimized for consistency\n";
    std::cout << "     - Reads optimized for speed\n";
    std::cout << "     - Clean separation of concerns\n";

    // ========================================================================
    // DEMONSTRATION: STORAGE STRATEGY (TIER-4 Feature #4)
    // ========================================================================
    std::cout << "\n[DEMO] Storage Strategy - Pluggable Architecture\n";
    std::cout << "────────────────────────────────────────────────\n";
    {
        auto& storageManager = StorageManager::instance();
        
        std::cout << "  Current Storage: " << storageManager.getStorageType() << "\n";
        std::cout << "  Storage Health: " << (storageManager.getStrategy().isHealthy() ? "✓" : "✗") << "\n";
        
        // Demonstrate abstraction
        std::cout << "  \n  Storage Operations (Abstract):\n";
        
        auto& storage = storageManager.getStrategy();
        
        // Create and save a menu item
        MenuItem burger;
        burger.id = "BURGER";
        burger.name = "Gourmet Burger";
        burger.price = 14.99;
        burger.quantityAvailable = 25;
        
        if (storage.saveMenuItem(burger)) {
            std::cout << "    ✓ Saved menu item via abstract interface\n";
        }
        
        // Load it back
        MenuItem loaded = storage.loadMenuItem("BURGER");
        std::cout << "    ✓ Loaded: " << loaded.name << " ($" 
                  << std::fixed << std::setprecision(2) << loaded.price << ")\n";
        
        std::cout << "  \n  Benefits of Strategy Pattern:\n";
        std::cout << "    - Easy to swap storage backend\n";
        std::cout << "    - Currently: CSV files\n";
        std::cout << "    - Future: SQLite, PostgreSQL, Cloud Storage\n";
        std::cout << "    - Zero business logic changes\n";
    }

    return 0;
}

