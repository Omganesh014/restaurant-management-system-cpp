/**
 * INTEGRATION ARCHITECTURE GUIDE
 * 
 * This file explains how the frontend and backend connect.
 * Reference this when building components or debugging issues.
 */

/**
 * SYSTEM OVERVIEW
 * 
 * ```
 * ┌──────────────────────────────────────────────────┐
 * │  REACT FRONTEND                                  │
 * │  ┌─────────────────────────────────────────────┐ │
 * │  │ Components (UI Display)                     │ │
 * │  └────────────────────┬────────────────────────┘ │
 * │                       │ (user actions)           │
 * │  ┌────────────────────▼────────────────────────┐ │
 * │  │ API Layer (src/api/*)                       │ │
 * │  │ - orderApi.ts                               │ │
 * │  │ - healthApi.ts                              │ │
 * │  │ - transactionApi.ts                         │ │
 * │  │ - apiClient.ts (transport)                  │ │
 * │  └────────────────────┬────────────────────────┘ │
 * │                       │ (JSON requests)          │
 * └───────────────────────┼──────────────────────────┘
 *                         │
 *         ┌───────────────┼───────────────┐
 *         │               │               │
 *    ┌────▼────┐  ┌──────▼──────┐  ┌───▼────┐
 *    │ HTTP    │  │ JSON Files  │  │ Mock   │
 *    │ (future)│  │ (optional)  │  │(dev)   │
 *    └─────────┘  └─────────────┘  └────────┘
 *         │               │               │
 *         └───────────────┼───────────────┘
 *                         │ (JSON responses)
 *                   ┌─────▼──────┐
 *                   │ C++ BACKEND│
 *                   │ ┌────────┐ │
 *                   │ │Orders  │ │
 *                   │ │Service │ │
 *                   │ └────────┘ │
 *                   │ ┌─────────────────────┐
 *                   │ │TransactionManager   │
 *                   │ │(ACID operations)    │
 *                   │ └─────────────────────┘
 *                   │ ┌─────────────────────┐
 *                   │ │HealthService       │
 *                   │ │(diagnostics)        │
 *                   │ └─────────────────────┘
 *                   │ ┌─────────────────────┐
 *                   │ │StorageStrategy      │
 *                   │ │(pluggable storage)  │
 *                   │ └─────────────────────┘
 *                   └────────────────────────┘
 * ```
 */

/**
 * FLOW 1: PLACE AN ORDER
 * 
 * 1. User clicks "Place Order" in React component
 * 2. Component calls orderApi.placeOrder({...})
 * 3. apiClient.request() → JSON request to backend
 * 4. Backend:
 *    - Validates menu items exist
 *    - Checks inventory availability
 *    - Calculates total
 *    - Wraps in Transaction (atomic)
 *    - Returns JSON response
 * 5. apiClient receives JSON and wraps in ApiResponse<OrderDTO>
 * 6. Component gets response, checks response.success
 * 7. If success: display new order, else show error
 * 
 * KEY: Frontend never validates business rules.
 *      Frontend just displays what backend decides.
 */

/**
 * FLOW 2: CONFIRM AN ORDER (State Transition)
 * 
 * Order FSM in BACKEND:
 * CREATED → CONFIRMED → PREPARING → READY → SERVED
 * 
 * User can only confirm if order.status === 'CREATED'
 * 
 * 1. User sees order in CREATED state
 * 2. Component disables "Confirm" button if status != 'CREATED'
 * 3. User clicks "Confirm"
 * 4. Component calls orderApi.confirmOrder(orderId)
 * 5. Backend:
 *    - Validates order exists
 *    - Validates current status is CREATED (FSM guard)
 *    - Reserves inventory (atomic)
 *    - Generates bill
 *    - Creates invoice (if needed)
 *    - Updates state to CONFIRMED
 *    - All wrapped in TransactionManager (all-or-nothing)
 *    - Returns new order state
 * 6. Component receives response
 * 7. If success: poll transaction status, update order state
 * 8. If failure: show error reason (e.g., "insufficient inventory")
 * 
 * KEY: FSM guard is in BACKEND, not frontend.
 *      Frontend respects backend's decision.
 */

/**
 * FLOW 3: HEALTH CHECK (Observability)
 * 
 * 1. On app startup, component calls healthApi.getSystemHealth()
 * 2. Backend runs self-checks:
 *    - Try to write then read from CSV (storage health)
 *    - Check snapshots directory exists
 *    - Verify event system initialized
 *    - Estimate memory usage
 * 3. Returns HealthCheckDTO:
 *    {
 *      status: 'HEALTHY' | 'DEGRADED' | 'UNHEALTHY',
 *      components: { csvOk, snapshotOk, eventsOk, ... }
 *    }
 * 4. Component displays status badge:
 *    - Green if HEALTHY
 *    - Yellow if DEGRADED
 *    - Red if UNHEALTHY
 * 5. User can click "Refresh" to re-check anytime
 * 
 * KEY: Frontend can't fix health issues, but can detect them.
 *      Shows user when to contact support.
 */

/**
 * FLOW 4: TRANSACTION TRACKING (Atomic Ops)
 * 
 * Some operations (confirm, cancel) may be slow because backend
 * performs multiple steps atomically. Component can track progress:
 * 
 * 1. Component calls orderApi.confirmOrder(orderId)
 * 2. Backend creates Transaction in TransactionManager:
 *    - status = ACTIVE
 *    - begins executing steps
 * 3. Backend returns:
 *    {
 *      success: true,
 *      data: { orderId, newStatus: 'CONFIRMED', ... },
 *      meta: { transactionId: 'TXN-123' }
 *    }
 * 4. Component polls transactionApi.getTransaction('TXN-123')
 * 5. Backend TransactionManager reports:
 *    {
 *      id: 'TXN-123',
 *      status: 'ACTIVE' | 'COMMITTED' | 'FAILED' | 'ROLLED_BACK',
 *      operationCount: 3,
 *      completedAt: '...' (if done)
 *    }
 * 6. Component shows spinner if ACTIVE, success/error when done
 * 7. If ROLLED_BACK or FAILED: show error + allow retry
 * 
 * KEY: Transaction IDs in meta allow correlation of operations.
 *      Useful for audit logging + debugging.
 */

/**
 * CQRS SPLIT (Read vs Write)
 * 
 * Backend has:
 * 
 * OrderCommandService (WRITES):
 * - createOrder()
 * - confirmOrder()
 * - cancelOrder()
 * - markAsServing()
 * 
 * OrderQueryService (READS):
 * - getActiveOrders()
 * - getOrder(id)
 * - getOrdersByCustomer(customerId)
 * - getOrdersByStatus(status)
 * 
 * Frontend mirrors this separation:
 * 
 * orderApi.placeOrder()        ← WRITE (may trigger transaction)
 * orderApi.confirmOrder()      ← WRITE (mutable operation)
 * 
 * orderApi.getActiveOrders()   ← READ (no side effects)
 * orderApi.getOrder(id)        ← READ (safe to call repeatedly)
 * 
 * BENEFIT: Reads can be cached, optimized for speed.
 *          Writes are optimized for consistency.
 */

/**
 * ERROR HANDLING CONTRACT
 * 
 * Backend always returns ApiResponse envelope:
 * 
 * SUCCESS:
 * {
 *   success: true,
 *   data: { ... },
 *   error: null
 * }
 * 
 * FAILURE (Business Rule):
 * {
 *   success: false,
 *   data: null,
 *   error: {
 *     code: 'BUSINESS_RULE_VIOLATION',
 *     message: 'Insufficient inventory for BURGER',
 *     details: { itemId, available, requested }
 *   }
 * }
 * 
 * FAILURE (Not Found):
 * {
 *   success: false,
 *   data: null,
 *   error: {
 *     code: 'NOT_FOUND',
 *     message: 'Order ORD-123 not found'
 *   }
 * }
 * 
 * FAILURE (Transport):
 * {
 *   success: false,
 *   data: null,
 *   error: {
 *     code: 'TRANSPORT_ERROR',
 *     message: 'Backend unavailable'
 *   }
 * }
 * 
 * Component always checks response.success before using data.
 * Displays error.message to user.
 */

/**
 * IDEMPOTENCY FOR SAFETY
 * 
 * Problem: User clicks "Place Order", network timeout.
 *          Did the order get created? User clicks again.
 *          Now there are 2 orders?
 * 
 * Solution: Client provides idempotencyKey
 * 
 * 1. Component generates:
 *    const idempotencyKey = `req-${userId}-${Date.now()}`;
 * 
 * 2. Sends with request:
 *    orderApi.placeOrder({
 *      customerId,
 *      items,
 *      idempotencyKey
 *    })
 * 
 * 3. Backend (IdempotencyService) checks:
 *    - If idempotencyKey seen before, return cached response
 *    - If new, execute order and cache result
 * 
 * 4. Component can safely retry on timeout:
 *    const response = await orderApi.placeOrder({ ..., idempotencyKey });
 *    if (!response.success && retry < maxRetries) {
 *      retry++;
 *      return orderApi.placeOrder({ ..., idempotencyKey }); // Safe!
 *    }
 * 
 * Result: Same order returned both times, no duplicates.
 */

/**
 * MOCK MODE (DEVELOPMENT)
 * 
 * apiClient can operate in 2 modes:
 * 
 * 1. MOCK (default)
 *    - Reads from mockResponses.ts
 *    - No backend needed
 *    - Instant responses
 *    - Perfect for UI development
 * 
 * 2. REAL
 *    - HTTP requests to backend
 *    - Backend validates everything
 *    - Slower but realistic
 * 
 * Switch mode:
 * ```typescript
 * import { apiClient } from './api/apiClient';
 * apiClient.setMockMode(false); // Use real backend
 * ```
 * 
 * Component code doesn't change—it's transport-agnostic!
 */

/**
 * TESTING GUIDE
 * 
 * UNIT TESTS (Component logic):
 * - Mock orderApi with jest.mock()
 * - Test that component calls correct API method
 * - Test that component displays response correctly
 * 
 * INTEGRATION TESTS (API + Backend):
 * - Use real backend running locally
 * - apiClient.setMockMode(false)
 * - Call full flow: place order → confirm → serve
 * - Verify backend state matches frontend
 * 
 * E2E TESTS (User flows):
 * - Cypress/Playwright
 * - Start both frontend and backend
 * - Simulate user actions
 * - Verify end-to-end behavior
 */

/**
 * INTERVIEW ANSWER
 * 
 * Q: "Walk me through how your React frontend connects to the C++ backend."
 * 
 * A: "The connection happens through a contract-driven API layer. The backend
 *     exposes functionality through JSON responses with a standard envelope
 *     containing success status, data, error details, and a transaction ID
 *     for correlation.
 * 
 *     The frontend's API client handles the transport abstraction. In
 *     development, it reads from mock data files. In production, it makes
 *     HTTP requests to the backend.
 * 
 *     I mirror the backend's architecture in the API layer. Since the
 *     backend uses CQRS (Command/Query Responsibility Segregation) and
 *     TransactionManager for atomic operations, the API layer has separate
 *     classes for reads vs writes. Reads are cheap and repeatable. Writes
 *     may trigger transactions, so the frontend can poll for completion
 *     using the transactionId.
 * 
 *     The key principle is: frontend gathers input and displays backend
 *     state. Backend validates everything, enforces the FSM, and manages
 *     transactions. This creates a clean boundary—if the backend rules
 *     change, the component code doesn't need to change."
 * 
 * Q: "Why separate read and write APIs?"
 * 
 * A: "Reads and writes have different optimization goals. Query methods
 *     (getActiveOrders) can be cached, paginated, optimized for speed.
 *     Command methods (confirmOrder) must be consistent and might wrap
 *     multiple steps in a transaction.
 * 
 *     By separating them in the API layer, the intent is clear in component
 *     code: use orderApi.getOrders() to display, use orderApi.confirmOrder()
 *     to mutate. The backend can then optimize each path independently."
 * 
 * Q: "What if a transaction fails during order confirmation?"
 * 
 * A: "The backend's TransactionManager ensures all-or-nothing execution.
 *     If confirmation requires [reserve inventory, generate bill, update state],
 *     and any step fails, all three roll back atomically.
 * 
 *     The API response will have status: FAILED or ROLLED_BACK. The component
 *     checks response.success, and if false, shows the error message to the
 *     user. The order remains in CREATED state, so the user can fix the issue
 *     and retry."
 */
