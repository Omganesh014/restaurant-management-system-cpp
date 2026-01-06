╔════════════════════════════════════════════════════════════════════════════════╗
║                         ELEVATOR PITCH - 30 SECONDS                              ║
╚════════════════════════════════════════════════════════════════════════════════╝

"I designed a production-grade C++ backend using FSMs, event-driven architecture, 
idempotency, and crash-recovery patterns, then exposed it as a frontend-agnostic 
service layer."

---

╔════════════════════════════════════════════════════════════════════════════════╗
║                     TECHNICAL DEPTH - 2 MINUTES                                ║
╚════════════════════════════════════════════════════════════════════════════════╝

I refactored a 3000-line monolithic C++ system into a clean, layered architecture:

TIER-1 (Foundation):
  • Configuration system with type-safe externalized settings
  • Permission hooks for RBAC compliance
  • Service locator pattern for dependency injection
  • Centralized business rules (20+ validations)

TIER-2 (Enterprise Patterns):
  • Event-driven architecture with 13 event types and 3 listeners
  • Soft-delete system for GDPR/HIPAA compliance
  • Idempotency service to prevent duplicate payments

TIER-3 (Advanced Capabilities):
  • Snapshot recovery for crash resilience
  • Command pattern with undo/redo and audit trails
  • Validation DSL for runtime rule configuration

Result: 10 features, 521 KB binary, 0 warnings, fully tested.

---

╔════════════════════════════════════════════════════════════════════════════════╗
║                         INTERVIEW ANGLE - 1 MINUTE                             ║
╚════════════════════════════════════════════════════════════════════════════════╝

"When I saw the monolith, I recognized three pain points:

1. RELIABILITY: System crashes lose in-flight orders → Built snapshot recovery
2. PAYMENT SAFETY: Network retries cause double-charges → Implemented idempotency
3. COMPLIANCE: Customer deletion concerns → Added soft-delete with audit trail

I designed a layered architecture where:
- Configuration doesn't require recompilation
- Services communicate via events (loose coupling)
- Every operation is atomic and reversible
- State transitions follow a strict FSM
- No external dependencies (portability first)

This demonstrates how to think holistically about backend design: 
identify pain points, architect solutions, implement systematically, 
and prepare for production constraints."

---

╔════════════════════════════════════════════════════════════════════════════════╗
║                    ARCHITECTURE DIAGRAM - 30 SECONDS                           ║
╚════════════════════════════════════════════════════════════════════════════════╝

            ┌─────────────────────────────────┐
            │      UI / Frontend              │
            └──────────────┬──────────────────┘
                           │
            ┌──────────────▼──────────────────┐
            │   SERVICE LOCATOR (DI)          │
            │   • OrderService                │
            │   • InventoryService            │
            │   • CustomerService             │
            │   • BillingService              │
            │   • AnalyticsService            │
            └──────────────┬──────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
        ▼                  ▼                  ▼
    ┌────────┐       ┌──────────┐      ┌──────────┐
    │ Events │       │Commands  │      │Snapshots │
    │ System │       │ Pattern  │      │Recovery  │
    └────────┘       └──────────┘      └──────────┘
        │                  │                  │
        └──────────────────┼──────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
        ▼                  ▼                  ▼
    ┌──────────┐  ┌─────────────┐   ┌──────────────┐
    │ Business │  │Permissions  │   │Config System │
    │  Rules   │  │ Enforcement │   │(Externalized)│
    └──────────┘  └─────────────┘   └──────────────┘
        │                  │                  │
        └──────────────────┼──────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
        ▼                  ▼                  ▼
    ┌────────┐       ┌────────────┐   ┌──────────┐
    │ Soft   │       │Idempotency │   │  Order   │
    │ Delete │       │ Service    │   │   FSM    │
    └────────┘       └────────────┘   └──────────┘
        │                  │                  │
        └──────────────────┼──────────────────┘
                           │
                    ┌──────▼──────┐
                    │Domain Models│
                    │ & Utilities │
                    └─────────────┘

---

╔════════════════════════════════════════════════════════════════════════════════╗
║                      KEY METRICS - WHAT TO EMPHASIZE                           ║
╚════════════════════════════════════════════════════════════════════════════════╝

✅ 10 Production Features
   └─ 4 TIER-1 (Foundational)
   └─ 3 TIER-2 (Enterprise)
   └─ 3 TIER-3 (Advanced)

✅ 7 Design Patterns
   └─ Singleton (ServiceLocator, EventBus, CommandInvoker)
   └─ Observer (EventSystem)
   └─ Service Locator (Dependency Injection)
   └─ Command (Undo/Redo/Replay)
   └─ State Machine (OrderFSM with 7 states)
   └─ Repository (Generic soft-delete)
   └─ Strategy (Validation DSL)

✅ 31 Files (Clean Structure)
   └─ 15 Headers (interfaces)
   └─ 14 Implementations (logic)
   └─ 1 Main demo
   └─ 1 Binary (521 KB)

✅ 3500+ Lines of Code
   └─ Zero external dependencies
   └─ 100% C++17 standard compliant
   └─ 0 warnings, 0 errors

✅ Professional Documentation
   └─ 6 markdown guides
   └─ API contract (frozen)
   └─ Architecture diagrams
   └─ Integration patterns
   └─ Quick reference

✅ Production Readiness
   └─ Configuration externalized (no recompilation)
   └─ Error handling comprehensive (12 error codes)
   └─ State machine enforced
   └─ Crash recovery via snapshots
   └─ Idempotency guaranteed
   └─ Soft deletes for compliance
   └─ Event-driven for loose coupling
   └─ Commands for audit trails

---

╔════════════════════════════════════════════════════════════════════════════════╗
║                       WHAT NOT TO MENTION                                      ║
╚════════════════════════════════════════════════════════════════════════════════╝

❌ Authentication/Authorization
   └─ Backend is agnostic, frontend responsibility

❌ Networking/HTTP APIs
   └─ In-process C++ backend, not HTTP server

❌ Databases/Persistence
   └─ CSV/TXT files only, state management on-disk

❌ Multi-threading/Async
   └─ Single-threaded synchronous design by choice

❌ External Frameworks/Libraries
   └─ Zero dependencies is a strength, not a limitation

❌ Performance Tuning
   └─ Optimized for correctness and clarity, not speed

---

╔════════════════════════════════════════════════════════════════════════════════╗
║                    COMMON INTERVIEW QUESTIONS                                  ║
╚════════════════════════════════════════════════════════════════════════════════╝

Q: "Why use state machines?"
A: "Order lifecycle has legal constraints (can't serve without confirming payment). 
   FSM enforces valid transitions at compile-time, prevents invalid states at runtime."

Q: "Why is idempotency important?"
A: "Payment processing is unreliable. Network timeouts cause retries. Without 
   idempotency, same payment processes twice. With it, duplicate requests return 
   cached result—user charged once, not twice. Critical for financial systems."

Q: "Why soft deletes instead of hard deletes?"
A: "Compliance (GDPR requires knowing what was deleted), audit (who deleted, when), 
   recovery (user changes mind, restore). Soft delete preserves history while 
   removing from active queries."

Q: "Why event-driven?"
A: "Services need to react to changes (order created → update inventory, log event, 
   notify user). Events decouple services (order service doesn't know about logging). 
   New listeners can be added without touching order service."

Q: "Why command pattern?"
A: "Every operation becomes reversible (undo), repeatable (replay), and auditable 
   (history). Critical for financial systems where every transaction must be trackable."

Q: "Why snapshot recovery?"
A: "System can crash. Snapshots save state to disk periodically. On restart, load 
   last snapshot → in-progress orders recover without data loss. Graceful degradation."

Q: "How would you add authentication?"
A: "That's a frontend responsibility. Backend defines permission actions. Frontend 
   authenticates user (token, session, OAuth). Frontend checks permissions before 
   calling backend. Backend enforces permissions if called directly."

Q: "How would you scale this?"
A: "This is in-process C++. To scale: API gateway (HTTP wrapper), distributed cache 
   (Redis for idempotency), message queue (Kafka for events), database (PostgreSQL 
   for persistence). Backend stays same, infrastructure changes."

Q: "What would you change in production?"
A: "Add configuration management (Consul), implement real database (PostgreSQL), 
   add monitoring/observability (Prometheus), switch to message queue (RabbitMQ), 
   add authentication layer (OAuth/JWT). This is the core business logic layer."

---

╔════════════════════════════════════════════════════════════════════════════════╗
║                         CLOSING STATEMENT                                      ║
╚════════════════════════════════════════════════════════════════════════════════╝

"This project demonstrates how I approach backend engineering: start with pain 
points, design solutions using proven patterns, implement systematically, and 
prepare for production. The code is clean, testable, and ready for frontend 
integration with zero dependencies on external frameworks."

---

Files to reference in interview:
  • BACKEND_API_CONTRACT.md ← Frozen API (professional approach)
  • QUICK_REFERENCE.md ← 10 features at a glance
  • tests/BackendTests.cpp ← Engineering maturity (testing)
  • ARCHITECTURE.md ← System design thinking
  • src/*.cpp ← Clean, readable implementations
  • main.cpp ← All features demonstrated live

When asked to show code: Start with main.cpp (demo all features), then 
explain one favorite feature (CommandPattern for undo, EventSystem for 
loose coupling, SnapshotManager for recovery, whatever you're most proud of).
