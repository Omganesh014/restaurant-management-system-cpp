# Backend Service Contract (STABLE API)

## Overview
This document defines the frozen API contract that any frontend (web, mobile, CLI, etc.) will depend on.
**Once frozen → No breaking changes without major version bump.**

---

## Service 1: OrderService

### Create Order
```cpp
Order createOrder(int customerId, std::vector<MenuItem> items, double totalAmount)
```
**Returns**: Order with CREATED state, unique orderId
**Throws**: RuntimeException if customer doesn't exist or amount invalid
**Side Effects**: Emits ORDER_PLACED event

### Confirm Order
```cpp
bool confirmOrder(int orderId)
```
**Returns**: true if transition to CONFIRMED succeeds
**Returns**: false if order in invalid state
**Side Effects**: Emits ORDER_CONFIRMED event

### Get Order
```cpp
Order getOrderById(int orderId)
```
**Returns**: Order object (including soft-deleted if found)
**Returns**: nullptr if not found

### Get Active Orders
```cpp
std::vector<Order> getActiveOrders()
```
**Returns**: Only non-deleted, non-served orders
**Used for**: Dashboard, kitchen display

### Update Order State
```cpp
bool updateOrderState(int orderId, OrderState newState)
```
**State Transitions**:
- CREATED → CONFIRMED → PREPARING → READY → SERVED ✅
- CREATED/CONFIRMED → CANCELLED (anytime) ✅
- SERVED → REFUNDED (within 7 days) ✅
- Invalid transitions → return false

**Side Effects**: Emits appropriate event (ORDER_CONFIRMED, ORDER_READY, etc.)

### Cancel Order
```cpp
bool cancelOrder(int orderId, std::string reason)
```
**Returns**: true if successful
**Idempotent**: Safe to call multiple times with same orderId
**Side Effects**: Emits ORDER_CANCELLED event

---

## Service 2: InventoryService

### Check Stock
```cpp
bool hasSufficientInventory(int itemId, int quantity)
```
**Returns**: true if stock >= quantity
**Returns**: false if insufficient

### Get Inventory
```cpp
MenuItem getInventoryItem(int itemId)
```
**Returns**: MenuItem with current stock level
**Returns**: nullptr if not found

### List All Items
```cpp
std::vector<MenuItem> getAllMenuItems(bool activeOnly = true)
```
**activeOnly=true**: Excludes soft-deleted items
**activeOnly=false**: Includes all items (admin view)

### Update Stock
```cpp
bool updateStock(int itemId, int quantity)
```
**Positive quantity**: Add to stock
**Negative quantity**: Reduce stock (if sufficient)
**Returns**: true if successful
**Side Effects**: Emits INVENTORY_UPDATED event (and INVENTORY_LOW if threshold breached)

### Soft Delete Item
```cpp
bool softDeleteItem(int itemId, std::string reason)
```
**Reason**: "Out of stock", "Discontinued", etc.
**Returns**: true if successful
**Effect**: Item no longer appears in getAllMenuItems() with activeOnly=true

### Restore Item
```cpp
bool restoreItem(int itemId)
```
**Returns**: true if item was deleted
**Returns**: false if item already active

---

## Service 3: CustomerService

### Create Customer
```cpp
Customer createCustomer(std::string name, std::string phone, std::string email)
```
**Returns**: Customer with unique customerId
**Validation**: Name non-empty, valid email format, valid phone
**Throws**: RuntimeException if validation fails

### Get Customer
```cpp
Customer getCustomerById(int customerId)
```
**Returns**: Customer object
**Returns**: nullptr if not found or soft-deleted

### Get All Customers
```cpp
std::vector<Customer> getAllCustomers(bool activeOnly = true)
```
**activeOnly=true**: Active customers only
**activeOnly=false**: Includes deleted customers

### Update Customer
```cpp
bool updateCustomer(int customerId, std::string name, std::string phone, std::string email)
```
**Returns**: true if successful
**Validation**: Same as createCustomer
**Idempotent**: Safe to call multiple times

### Soft Delete Customer
```cpp
bool softDeleteCustomer(int customerId, std::string reason)
```
**Reason**: "Requested closure", "Inactive", etc.
**Returns**: true if successful
**Effect**: Customer no longer appears in getAllCustomers() with activeOnly=true
**GDPR Compliance**: Data preserved for 90 days

### Restore Customer
```cpp
bool restoreCustomer(int customerId)
```
**Returns**: true if customer was deleted
**Returns**: false if already active

---

## Service 4: BillingService

### Generate Bill
```cpp
Bill generateBill(int orderId)
```
**Returns**: Bill with itemized breakdown, taxes, total
**Calculation**: Sum(items) + tax (18%)
**Side Effects**: Emits no event (billing is internal)

### Process Payment
```cpp
bool processPayment(int billId, std::string method, std::string requestId)
```
**method**: "CASH", "CARD", "DIGITAL_WALLET"
**requestId**: Idempotency key (prevent double-charge on network retry)
**Returns**: true if payment successful
**Throws**: RuntimeException if payment fails
**Side Effects**: Emits PAYMENT_PROCESSED event
**Idempotency**: Same requestId → returns cached result (no double-charge)

### Get Bill
```cpp
Bill getBillById(int billId)
```
**Returns**: Bill with payment status
**Returns**: nullptr if not found

### Get Bills for Order
```cpp
std::vector<Bill> getBillsByOrderId(int orderId)
```
**Returns**: All bills associated with order (can be multiple for refunds)

---

## Service 5: AnalyticsService

### Get Daily Revenue
```cpp
double getDailyRevenue(std::string date = "TODAY")
```
**Returns**: Sum of all completed payments for the day
**Format**: Numeric total in currency

### Get Active Orders Count
```cpp
int getActiveOrdersCount()
```
**Returns**: Count of orders in state CONFIRMED, PREPARING, or READY

### Get Customer Count
```cpp
int getTotalCustomerCount(bool activeOnly = true)
```
**activeOnly=true**: Active customers only
**activeOnly=false**: Includes deleted customers

### Get Low Inventory Items
```cpp
std::vector<MenuItem> getLowInventoryItems(int threshold = 10)
```
**threshold**: Stock level below which item is considered "low"
**Returns**: Items currently below threshold

### Get Order by State
```cpp
int getOrderCountByState(OrderState state)
```
**Returns**: Count of orders in specific state

---

## Global Configuration (Read-Only After Startup)

### Configuration Access
```cpp
int Config::getInt(std::string key)
double Config::getDouble(std::string key)
std::string Config::getString(std::string key)
bool Config::getBool(std::string key)
```

**Available Keys**:
- `MAX_CUSTOMERS`: Maximum customers allowed (default: 500)
- `MAX_ORDERS_PER_DAY`: Daily order limit (default: 1000)
- `TAX_RATE`: Sales tax rate (default: 0.18 = 18%)
- `REFUND_WINDOW_DAYS`: Days within which refund is allowed (default: 7)
- `IDEMPOTENCY_TTL_HOURS`: How long to cache requests (default: 24)
- `LOG_LEVEL`: Logging verbosity (default: "INFO")

**Important**: Configuration is frozen at startup. Changes require system restart.

---

## Event Subscriptions (Observer Pattern)

### Subscribable Events
```cpp
enum EventType {
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
```

### Subscribe to Events
```cpp
class MyListener : public EventListener {
    void onEvent(const Event& evt) override {
        // Handle event
    }
};

EventBus::getInstance().subscribe(new MyListener());
```

**Guaranteed**: All listeners are called synchronously when event emitted.
**Thread Safety**: No guarantees (single-threaded system).

---

## Idempotency Guarantee

### Idempotent Operations

Any operation that accepts `requestId` parameter is idempotent:

```cpp
// First call
processPayment(billId, "CARD", "req-2024-001");  // Executes, caches result

// Retry (network error, user retried)
processPayment(billId, "CARD", "req-2024-001");  // Returns cached result, no double-charge
```

**TTL**: 24 hours (configurable via IDEMPOTENCY_TTL_HOURS)
**Expiry**: After TTL, same requestId treated as new request

---

## State Machine Transitions (Guaranteed)

### Order State Diagram
```
CREATED
  ├─→ CONFIRMED
  │     ├─→ PREPARING
  │     │     ├─→ READY
  │     │     │     ├─→ SERVED
  │     │     │     │     └─→ REFUNDED (within 7 days)
  │     │     │     └─→ CANCELLED
  │     │     └─→ CANCELLED
  │     └─→ CANCELLED
  └─→ CANCELLED
```

**Invalid transitions**: Blocked, return false
**Example**: SERVED → PREPARING = INVALID ❌

---

## Error Handling

### Exception Hierarchy
```cpp
std::runtime_error         // Base class for all backend errors
├─ PermissionException    // Permission denied
├─ ValidationException    // Input validation failed
├─ StateException         // Invalid state transition
└─ NotFoundException      // Resource not found
```

### Standard Responses
```cpp
bool operation()           // Returns false if failed
Order getOrder()           // Returns nullptr if not found
throws RuntimeException    // For validation/permission failures
```

---

## Backwards Compatibility

### Version: 1.0 (Current - Frozen)

**Stable Until**: Breaking change (major version bump)

**What Won't Change**:
- Method signatures for all listed services
- Parameter names and types
- Return types
- EventTypes enum
- Configuration keys
- State machine transitions

**What Can Change** (without breaking version):
- Implementation details (internal refactoring)
- Performance improvements
- New fields in response objects (additive only)
- New optional parameters (with defaults)
- New events (additive, no removal)

---

## Testing Contract

### Expected Behaviors

1. **Order Creation**
   - Valid order → returns Order with CREATED state
   - Invalid customer ID → throws exception
   - Negative amount → throws exception

2. **State Transitions**
   - CREATED → CONFIRMED → SERVING → READY → SERVED ✅
   - SERVED → PREPARING ❌ (throws exception or returns false)

3. **Idempotency**
   - Same requestId within TTL → returns cached result
   - Same requestId after TTL → processes as new request

4. **Events**
   - placeOrder() → emits ORDER_PLACED event
   - confirmOrder() → emits ORDER_CONFIRMED event
   - Events reach all registered listeners

5. **Soft Delete**
   - softDeleteCustomer() → customer not in getAllCustomers()
   - restoreCustomer() → customer reappears in getAllCustomers()
   - getCustomerById() → returns deleted customer (if exists)

---

## Integration Checklist

Before frontend depends on backend:

- [ ] All 5 services available and tested
- [ ] All events emitting correctly
- [ ] Idempotency working (no double-charges)
- [ ] State transitions enforced
- [ ] Soft deletes working
- [ ] Snapshots can be created/restored
- [ ] Commands can be executed/undone
- [ ] Validation DSL accepting custom rules
- [ ] Configuration loading from file
- [ ] Permission checks enforced

---

## Conclusion

This contract is **FROZEN** as of January 7, 2026.

Frontend developers can depend on these APIs without fear of breaking changes.

Backend implementation details can evolve, but this interface is stable.
