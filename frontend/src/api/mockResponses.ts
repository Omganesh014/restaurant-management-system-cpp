/**
 * MOCK DATA / EXAMPLE RESPONSES
 * 
 * These are realistic responses that the backend would return.
 * Use these to develop UI without a running backend.
 * 
 * When backend is ready, just change apiClient.setMockMode(false)
 */

// ============================================================================
// HEALTH CHECK RESPONSE
// ============================================================================
export const MOCK_HEALTH_HEALTHY = {
  success: true,
  data: {
    status: 'HEALTHY',
    csvAccessible: true,
    snapshotAccessible: true,
    eventSystemOperational: true,
    servicesInitialized: true,
    estimatedMemoryMB: 45,
    snapshotCount: 3,
    diagnostics:
      'All systems operational. CSV storage writable. 3 snapshots available.',
  },
  error: null,
  meta: {
    timestamp: '2025-01-07T14:32:15.123Z',
    transactionId: 'TXN-20250107-health-01',
  },
};

export const MOCK_HEALTH_DEGRADED = {
  success: true,
  data: {
    status: 'DEGRADED',
    csvAccessible: true,
    snapshotAccessible: false,
    eventSystemOperational: true,
    servicesInitialized: true,
    estimatedMemoryMB: 89,
    snapshotCount: 0,
    diagnostics:
      'WARNING: Snapshots directory not accessible. Crash recovery disabled.',
  },
  error: null,
  meta: {
    timestamp: '2025-01-07T14:32:15.123Z',
    transactionId: 'TXN-20250107-health-02',
  },
};

// ============================================================================
// ORDER RESPONSES
// ============================================================================
export const MOCK_ORDER_CREATED = {
  success: true,
  data: {
    id: 'ORD-20250107-001',
    customerId: 'CUST-001',
    items: [
      {
        itemId: 'BURGER',
        name: 'Gourmet Burger',
        price: 12.99,
        quantity: 2,
        subtotal: 25.98,
      },
      {
        itemId: 'FRIES',
        name: 'Golden Fries',
        price: 4.99,
        quantity: 2,
        subtotal: 9.98,
      },
    ],
    status: 'CREATED',
    total: 35.96,
    createdAt: '2025-01-07T14:30:00.000Z',
    updatedAt: '2025-01-07T14:30:00.000Z',
  },
  error: null,
  meta: {
    timestamp: '2025-01-07T14:30:05.123Z',
    transactionId: 'TXN-20250107-create-order-001',
  },
};

export const MOCK_ORDER_CONFIRMED = {
  success: true,
  data: {
    ...MOCK_ORDER_CREATED.data,
    status: 'CONFIRMED',
    updatedAt: '2025-01-07T14:31:00.000Z',
  },
  error: null,
  meta: {
    timestamp: '2025-01-07T14:31:05.123Z',
    transactionId: 'TXN-20250107-confirm-order-001',
  },
};

export const MOCK_ACTIVE_ORDERS = {
  success: true,
  data: {
    orders: [
      {
        id: 'ORD-20250107-001',
        customerId: 'CUST-001',
        items: [
          {
            itemId: 'BURGER',
            name: 'Gourmet Burger',
            price: 12.99,
            quantity: 2,
            subtotal: 25.98,
          },
        ],
        status: 'CONFIRMED',
        total: 25.98,
        createdAt: '2025-01-07T14:00:00.000Z',
        updatedAt: '2025-01-07T14:10:00.000Z',
      },
      {
        id: 'ORD-20250107-002',
        customerId: 'CUST-002',
        items: [
          {
            itemId: 'PIZZA',
            name: 'Pepperoni Pizza',
            price: 18.99,
            quantity: 1,
            subtotal: 18.99,
          },
        ],
        status: 'READY',
        total: 18.99,
        createdAt: '2025-01-07T13:45:00.000Z',
        updatedAt: '2025-01-07T14:25:00.000Z',
      },
    ],
    count: 2,
  },
  error: null,
  meta: {
    timestamp: '2025-01-07T14:32:15.123Z',
    transactionId: 'TXN-20250107-list-active-001',
  },
};

export const MOCK_CANCEL_ORDER = {
  success: true,
  data: {
    orderId: 'ORD-20250107-001',
    newStatus: 'CANCELLED',
    refundAmount: 25.98,
    transactionId: 'TXN-20250107-cancel-order-001',
  },
  error: null,
  meta: {
    timestamp: '2025-01-07T14:35:15.123Z',
    transactionId: 'TXN-20250107-cancel-order-001',
  },
};

// ============================================================================
// TRANSACTION RESPONSES
// ============================================================================
export const MOCK_TRANSACTION_COMMITTED = {
  success: true,
  data: {
    id: 'TXN-20250107-create-order-001',
    status: 'COMMITTED',
    operationCount: 3,
    createdAt: '2025-01-07T14:30:00.000Z',
    completedAt: '2025-01-07T14:30:02.000Z',
  },
  error: null,
  meta: {
    timestamp: '2025-01-07T14:30:05.123Z',
    transactionId: 'TXN-20250107-create-order-001',
  },
};

export const MOCK_TRANSACTION_FAILED = {
  success: true,
  data: {
    id: 'TXN-20250107-confirm-order-002',
    status: 'FAILED',
    operationCount: 1,
    createdAt: '2025-01-07T14:40:00.000Z',
    completedAt: '2025-01-07T14:40:01.000Z',
  },
  error: null,
  meta: {
    timestamp: '2025-01-07T14:40:05.123Z',
    transactionId: 'TXN-20250107-confirm-order-002',
  },
};

export const MOCK_TRANSACTION_ROLLED_BACK = {
  success: true,
  data: {
    id: 'TXN-20250107-billing-001',
    status: 'ROLLED_BACK',
    operationCount: 3,
    createdAt: '2025-01-07T14:45:00.000Z',
    completedAt: '2025-01-07T14:45:01.500Z',
  },
  error: null,
  meta: {
    timestamp: '2025-01-07T14:45:05.123Z',
    transactionId: 'TXN-20250107-billing-001',
  },
};

// ============================================================================
// ERROR RESPONSES
// ============================================================================
export const MOCK_ERROR_ORDER_NOT_FOUND = {
  success: false,
  data: null,
  error: {
    code: 'NOT_FOUND',
    message: 'Order ORD-20250107-999 not found',
    details: {
      orderId: 'ORD-20250107-999',
    },
  },
  meta: {
    timestamp: '2025-01-07T14:50:15.123Z',
    transactionId: 'TXN-20250107-get-order-999',
  },
};

export const MOCK_ERROR_INSUFFICIENT_INVENTORY = {
  success: false,
  data: null,
  error: {
    code: 'BUSINESS_RULE_VIOLATION',
    message: 'Insufficient inventory for BURGER',
    details: {
      itemId: 'BURGER',
      available: 2,
      requested: 5,
    },
  },
  meta: {
    timestamp: '2025-01-07T14:51:15.123Z',
    transactionId: 'TXN-20250107-create-order-002',
  },
};

export const MOCK_ERROR_INVALID_STATE_TRANSITION = {
  success: false,
  data: null,
  error: {
    code: 'BUSINESS_RULE_VIOLATION',
    message: 'Cannot confirm order in SERVED state',
    details: {
      orderId: 'ORD-20250107-001',
      currentStatus: 'SERVED',
      attemptedAction: 'CONFIRM',
    },
  },
  meta: {
    timestamp: '2025-01-07T14:52:15.123Z',
    transactionId: 'TXN-20250107-confirm-order-003',
  },
};

export const MOCK_ERROR_BACKEND_UNAVAILABLE = {
  success: false,
  data: null,
  error: {
    code: 'TRANSPORT_ERROR',
    message: 'Backend service unavailable',
  },
  meta: {
    timestamp: new Date().toISOString(),
    transactionId: `TXN-${Date.now()}`,
  },
};
