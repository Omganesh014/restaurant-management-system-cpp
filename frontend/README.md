# Restaurant System Frontend

React frontend for a full-stack restaurant ordering system. **Presentation-only** — all business logic lives in the C++ backend.

## Architecture Principle

```
React UI  ↔  Contract Layer  ↔  C++ Backend
(Display)    (API Client)      (Logic + State)
```

**Frontend responsibility:** Gather user input, display backend state, handle transport errors.

**Backend responsibility:** Validate, enforce FSM, manage transactions, track health.

## Frontend API Layer

Located in `src/api/`, organized by domain:

```
src/api/
├── apiClient.ts          # Transport abstraction (HTTP / mock)
├── types.ts              # API contract types (shared with backend)
├── orderApi.ts           # Order operations (CQRS)
├── healthApi.ts          # System diagnostics
├── transactionApi.ts     # Transaction status tracking
├── mockResponses.ts      # Mock data for development
└── useBackend.example.tsx # Component usage examples
```

### API Contract Format

All responses follow this envelope:

```typescript
interface ApiResponse<T> {
  success: boolean;
  data: T | null;
  error: { code: string; message: string } | null;
  meta: {
    timestamp: string;
    transactionId: string;  // For audit/correlation
  };
}
```

### Key APIs

#### Orders (CQRS)

```typescript
// READS (Query Service)
orderApi.getActiveOrders()         // Get all non-terminal orders
orderApi.getCustomerOrders(id)     // Filter by customer
orderApi.getOrder(orderId)         // Single order

// WRITES (Command Service)
orderApi.placeOrder(request)       // Create (returns CREATED state)
orderApi.confirmOrder(orderId)     // Transition to CONFIRMED
orderApi.cancelOrder(orderId)      // Request cancellation
orderApi.markAsServed(orderId)     // Kitchen → Cashier
```

#### Health (Observability)

```typescript
healthApi.getSystemHealth()        // Full diagnostic report
healthApi.isBackendReady()         // Boolean ready check
```

#### Transactions (ACID Tracking)

```typescript
transactionApi.getTransaction(id)       // Get status
transactionApi.getRecentTransactions()  // Audit log
transactionApi.isSuccessful(tx)         // Helper
transactionApi.isFailed(tx)             // Helper
```

### Backend-Enforced Rules

The backend enforces these—frontend must respect them:

1. **FSM State Machine**: Orders have legal state transitions
   - Cannot skip states (CREATED → SERVED requires intermediate steps)
   - Cannot transition backward

2. **Inventory Consistency**: Confirmations reserve stock atomically
   - If inventory check fails, entire operation rolls back
   - UI gets error, must show to user

3. **Idempotency**: Duplicate requests return same result
   - Frontend includes `idempotencyKey` in create requests
   - Safe to retry on network timeout

4. **Transaction Atomicity**: Multi-step operations all-or-nothing
   - Order creation wraps [create, reserve inventory, bill] in transaction
   - Any step fails → all roll back

### Development Mode (Mocked Backend)

By default, the API client runs in **mock mode**:

```typescript
// src/api/apiClient.ts
const apiClient = new ApiClient({
  mockMode: true,  // Read from mock data
});
```

Mock responses are defined in `src/api/mockResponses.ts`.

To use a real backend:

```typescript
apiClient.setMockMode(false);
// Expects backend at http://localhost:3001
```

### Component Usage Patterns

**Do:**

```typescript
// Fetch + display in component
const [orders, setOrders] = useState([]);
useEffect(() => {
  const fetch = async () => {
    const response = await orderApi.getActiveOrders();
    if (response.success) {
      setOrders(response.data.orders);
    }
  };
  fetch();
}, []);

// Display based on backend state
<button disabled={order.status !== 'CREATED'}>
  Confirm
</button>
```

**Don't:**

```typescript
// ❌ Duplicate FSM logic
if (order.status === 'CREATED' && order.total > 100) {
  // Don't validate here—let backend do it
}

// ❌ Assume operation succeeded
const response = await orderApi.placeOrder(...);
setOrders([...orders, newOrder]); // No! Check response.success first

// ❌ Show stale data
// Always fetch fresh state after mutations
```

## Running the App

**Prerequisites:** Node.js 18+

```bash
# Install dependencies
npm install

# Development server (mock backend)
npm run dev

# Build for production
npm npm run build

# Preview production build
npm run preview
```

## File Structure

```
frontend/
├── src/
│   ├── api/                    # Backend integration
│   │   ├── apiClient.ts        # Transport layer
│   │   ├── types.ts            # API contracts
│   │   ├── orderApi.ts         # Order operations
│   │   ├── healthApi.ts        # Health checks
│   │   ├── transactionApi.ts   # Transaction tracking
│   │   └── mockResponses.ts    # Development data
│   ├── components/             # React UI
│   ├── App.tsx                 # Root component
│   └── index.tsx               # Entry point
├── public/                     # Static assets
├── package.json                # Dependencies
├── tsconfig.json               # TypeScript config
└── vite.config.ts              # Build config
```

## Backend Integration Testing

To test against the real C++ backend:

1. Ensure backend is running (outputs JSON to `data/` or via HTTP)
2. Switch to real mode:
   ```typescript
   apiClient.setMockMode(false);
   ```
3. Update `apiClient` base URL if needed
4. Run `npm run dev` and test user flows

## Key Design Decisions

| Decision | Rationale |
|----------|-----------|
| **API layer isolated** | Changes to backend don't break components |
| **Type-safe contracts** | TypeScript prevents misuse of API |
| **Mock mode default** | Develop UI without backend running |
| **CQRS reads/writes split** | Mirrors backend architecture |
| **Transaction tracking** | Users see operation progress |
| **No Redux/Context** | Simple local state, fetch on demand |
| **Error codes from backend** | Centralized, meaningful error messages |

## Interview Talking Points

**Q: "How does your React app work without a backend server?"**

A: "It uses a contract-driven adapter layer. The backend (C++) outputs JSON to files or stdout. The frontend's API client handles the transport abstraction, so I can swap from mock files to HTTP without changing component code. The real value is the clear boundary: frontend asks, backend decides."

**Q: "Why CQRS in the API layer?"**

A: "Read and write operations have different requirements. Queries are side-effect-free, can be repeated safely. Commands may trigger transactions, inventory updates, events. By separating them in the API layer, I mirror the backend's architecture and make the intent clear in component code."

**Q: "What happens if a transaction fails?"**

A: "The backend's TransactionManager rolls back all steps atomically. The API returns `status: FAILED` or `status: ROLLED_BACK`. The component sees this, shows an error message, and disables the action so the user can retry."
