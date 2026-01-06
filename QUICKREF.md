# Quick Reference Guide

## Build

```bash
"C:\msys64\usr\bin\bash.exe" -lc "cd /d/daa_project_fullstack && g++ -std=c++17 -Wall -O2 src/*.cpp main.cpp -Iinclude -o restaurant_system.exe"
```

## Run

```bash
.\restaurant_system.exe
```

---

## Code Snippets

### Use Configuration
```cpp
#include "Config.h"

Config::initialize("config/config.txt");
int max = Config::getInt("MAX_ORDERS", 300);
double tax = Config::getDouble("TAX_RATE", 0.18);
```

### Check Permissions
```cpp
#include "PermissionService.h"

PermissionService::enforce(Action::CREATE_ORDER);
if (PermissionService::canPerform(Action::ISSUE_REFUND)) { ... }
```

### Access Services
```cpp
#include "ServiceLocator.h"

ServiceLocator::initialize();
OrderService& orders = ServiceLocator::order();
InventoryService& inv = ServiceLocator::inventory();
```

### Validate Business Rules
```cpp
#include "BusinessRules.h"

if (!BusinessRules::canCancelOrder(order)) {
    cout << BusinessRules::getViolationMessage() << endl;
}

if (BusinessRules::needsReorder(item, qty, level)) {
    cout << "Reorder required!" << endl;
}
```

### Emit Events
```cpp
#include "EventSystem.h"

Event e{
    EventType::ORDER_PLACED, 
    orderId, 
    "Order", 
    "Customer placed order",
    time(nullptr),
    "OrderService"
};
EventBus::getInstance().emit(e);
```

### Create Custom Event Listener
```cpp
class MyListener : public EventListener {
public:
    void onEvent(const Event& e) override {
        cout << "Event received: " << e.entityType << endl;
    }
    std::string getName() const override { return "MyListener"; }
};

MyListener* listener = new MyListener();
EventBus::getInstance().subscribe(listener);
```

### Soft Delete
```cpp
#include "SoftDelete.h"

CustomerRecord customer{...};
customer.softDelete("Requested account closure");  // Mark inactive
// customer is not deleted, data is preserved

customer.restore();  // Reactivate
```

### Prevent Duplicate Operations
```cpp
#include "IdempotencyService.h"

std::string requestId = "req-2024-001";
std::string cachedResult;

if (IdempotencyService::isDuplicate(requestId, cachedResult)) {
    return cachedResult;  // Return cached result
}

// Process request
std::string result = placeOrder(order);
IdempotencyService::recordSuccess(requestId, "place_order", result);
return result;
```

---

## Configuration File (config/config.txt)

```ini
MAX_CUSTOMERS=500
MAX_ORDERS=300
MAX_MENU_ITEMS=200
TAX_RATE=0.18
SERVICE_CHARGE_PERCENT=5.0
LOG_LEVEL=INFO
BACKUP_INTERVAL_MIN=30
REFUND_WINDOW_DAYS=7
```

---

## File Organization

| Directory | Purpose |
|-----------|---------|
| `include/` | All headers |
| `src/` | All implementations |
| `config/` | Configuration files |
| `data/` | CSV data files |

---

## Key Classes

### Config
```cpp
Config::initialize(filename);
Config::getInt(key, defaultValue);
Config::getDouble(key, defaultValue);
Config::getString(key, defaultValue);
Config::exists(key);
```

### PermissionService
```cpp
PermissionService::canPerform(action);
PermissionService::enforce(action);  // Throws if denied
PermissionService::actionToString(action);
```

### ServiceLocator
```cpp
ServiceLocator::initialize();
ServiceLocator::order();
ServiceLocator::inventory();
ServiceLocator::analytics();
ServiceLocator::delivery();
ServiceLocator::cleanup();
```

### BusinessRules
```cpp
BusinessRules::canCreateOrder(customerId, amount);
BusinessRules::canModifyOrder(order);
BusinessRules::canCancelOrder(order);
BusinessRules::canRefundOrder(order);
BusinessRules::calculateTotalWithTax(subtotal);
BusinessRules::getViolationMessage();
```

### EventBus
```cpp
EventBus::getInstance().subscribe(listener);
EventBus::getInstance().emit(event);
EventBus::getInstance().unsubscribe(listener);
```

### IdempotencyService
```cpp
IdempotencyService::isDuplicate(requestId, cachedResult);
IdempotencyService::recordSuccess(requestId, opType, result);
IdempotencyService::recordFailure(requestId, opType);
IdempotencyService::getRecord(requestId);
IdempotencyService::cleanupExpired();
IdempotencyService::setDefaultTTL(seconds);
```

---

## Testing Checklist

- [ ] Build compiles without errors
- [ ] Binary runs without crashes
- [ ] Config loads successfully
- [ ] Permissions enforce correctly
- [ ] Services initialize/cleanup
- [ ] Business rules validate correctly
- [ ] FSM transitions work
- [ ] Events emit to all listeners
- [ ] Soft deletes preserve data
- [ ] Idempotent service prevents duplicates

---

## Troubleshooting

**Binary won't compile:**
- Ensure MSYS2 is installed
- Check gcc version: `g++ --version`
- Verify all headers are in `include/`
- Verify all `.cpp` files are in `src/`

**Binary runs but segfaults:**
- Check ServiceLocator::initialize() is called first
- Ensure EventBus::getInstance() isn't accessed before EventSystem.cpp
- Verify listeners are cleaned up before EventBus

**Config not loading:**
- Check file exists: `config/config.txt`
- Verify format: `KEY=VALUE` (no spaces around =)
- Check permissions on file

---

## Next Steps

1. Extend `OrderService`, `InventoryService`, etc. with real logic
2. Create CSV I/O using Config paths
3. Implement `SoftDeleteRepository` for actual database layer
4. Add RBAC plugin to PermissionService
5. Create integration tests using ServiceLocator

---

**Version:** 2.1  
**Last Updated:** Jan 7, 2026
