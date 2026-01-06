/**
 * Example: Using the API Layer in React Components
 * 
 * This file is DOCUMENTATION + EXAMPLE PATTERNS.
 * Copy-paste these patterns into your component files.
 * 
 * KEY PRINCIPLE:
 * Components fetch data → display it → listen for state changes from backend.
 * 
 * NO business logic in components.
 */

import { useState, useEffect } from 'react';
import { orderApi } from './orderApi';
import { healthApi } from './healthApi';
import { transactionApi } from './transactionApi';
import { OrderDTO, HealthCheckDTO, TransactionDTO } from './types';

/**
 * EXAMPLE 1: Fetch and display active orders
 */
export const ExampleOrdersList = () => {
  const [orders, setOrders] = useState<OrderDTO[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    const fetch = async () => {
      setLoading(true);
      const response = await orderApi.getActiveOrders();
      setLoading(false);

      if (response.success && response.data) {
        setOrders(response.data.orders);
        setError(null);
      } else {
        setError(response.error?.message || 'Failed to fetch orders');
      }
    };

    fetch();
  }, []);

  if (loading) return <p>Loading orders...</p>;
  if (error) return <p style={{ color: 'red' }}>{error}</p>;

  return (
    <div>
      <h2>Active Orders ({orders.length})</h2>
      {orders.map((order) => (
        <div
          key={order.id}
          style={{
            border: '1px solid #ccc',
            padding: '10px',
            marginBottom: '10px',
          }}
        >
          <strong>{order.id}</strong> - {order.status} (${order.total})
          <p>Customer: {order.customerId}</p>
          <p>Items: {order.items.length}</p>
        </div>
      ))}
    </div>
  );
};

/**
 * EXAMPLE 2: Health check dashboard
 */
export const ExampleHealthDashboard = () => {
  const [health, setHealth] = useState<HealthCheckDTO | null>(null);
  const [loading, setLoading] = useState(false);

  const checkHealth = async () => {
    setLoading(true);
    const response = await healthApi.getSystemHealth();
    setLoading(false);

    if (response.success && response.data) {
      setHealth(response.data);
    }
  };

  useEffect(() => {
    checkHealth();
  }, []);

  if (!health) return <p>Loading health status...</p>;

  const components = healthApi.getComponentStatus(health);
  const statusColor =
    health.status === 'HEALTHY'
      ? 'green'
      : health.status === 'DEGRADED'
      ? 'orange'
      : 'red';

  return (
    <div>
      <h2>System Health</h2>
      <div
        style={{
          padding: '10px',
          backgroundColor: statusColor,
          color: 'white',
          borderRadius: '5px',
          marginBottom: '20px',
        }}
      >
        <strong>{health.status}</strong>: {healthApi.getStatusMessage(health)}
      </div>

      <h3>Components</h3>
      {components.map((comp) => (
        <div key={comp.name} style={{ marginBottom: '10px' }}>
          <span>{comp.name}: </span>
          <span style={{ color: comp.ok ? 'green' : 'red' }}>
            {comp.ok ? '✓ OK' : '✗ ERROR'}
          </span>
        </div>
      ))}

      <p>Memory: {health.estimatedMemoryMB} MB</p>
      <p>Snapshots: {health.snapshotCount}</p>

      <button onClick={checkHealth} disabled={loading}>
        {loading ? 'Checking...' : 'Refresh'}
      </button>
    </div>
  );
};

/**
 * EXAMPLE 3: Create order and track transaction
 */
export const ExampleCreateOrder = () => {
  const [loading, setLoading] = useState(false);
  const [transactionId, setTransactionId] = useState<string | null>(null);
  const [transaction, setTransaction] = useState<TransactionDTO | null>(null);

  const handleCreateOrder = async () => {
    setLoading(true);

    const response = await orderApi.placeOrder({
      customerId: 'CUST-001',
      items: [{ itemId: 'BURGER', quantity: 2 }],
      idempotencyKey: `req-${Date.now()}`,
    });

    setLoading(false);

    if (response.success && response.meta) {
      setTransactionId(response.meta.transactionId);
      // Poll transaction status
      pollTransaction(response.meta.transactionId);
    }
  };

  const pollTransaction = async (txnId: string, attempts = 0) => {
    if (attempts > 5) return; // Stop after 5 attempts

    const response = await transactionApi.getTransaction(txnId);
    if (response.success && response.data) {
      setTransaction(response.data);

      // Keep polling if still processing
      if (transactionApi.isProcessing(response.data)) {
        setTimeout(() => pollTransaction(txnId, attempts + 1), 500);
      }
    }
  };

  return (
    <div>
      <button onClick={handleCreateOrder} disabled={loading}>
        {loading ? 'Creating...' : 'Create Order'}
      </button>

      {transaction && (
        <div style={{ marginTop: '20px', padding: '10px', border: '1px solid #ccc' }}>
          <h3>Transaction: {transactionId}</h3>
          <p>Status: {transactionApi.getStatusMessage(transaction)}</p>
          <p>Operations: {transaction.operationCount}</p>
          {transactionApi.isSuccessful(transaction) && (
            <p style={{ color: 'green' }}>✓ Order created successfully!</p>
          )}
          {transactionApi.isFailed(transaction) && (
            <p style={{ color: 'red' }}>✗ Order creation failed.</p>
          )}
        </div>
      )}
    </div>
  );
};

/**
 * EXAMPLE 4: Confirm order (state transition)
 */
export const ExampleConfirmOrder = ({ orderId }: { orderId: string }) => {
  const [loading, setLoading] = useState(false);
  const [success, setSuccess] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const handleConfirm = async () => {
    setLoading(true);
    const response = await orderApi.confirmOrder(orderId);
    setLoading(false);

    if (response.success) {
      setSuccess(true);
      setError(null);
      // Refetch order or update state here
    } else {
      setError(response.error?.message || 'Failed to confirm order');
      setSuccess(false);
    }
  };

  return (
    <div>
      <button onClick={handleConfirm} disabled={loading}>
        {loading ? 'Confirming...' : 'Confirm Order'}
      </button>

      {success && <p style={{ color: 'green' }}>✓ Order confirmed!</p>}
      {error && <p style={{ color: 'red' }}>✗ {error}</p>}
    </div>
  );
};

/**
 * KEY PATTERNS DEMONSTRATED:
 *
 * 1. FETCHING DATA
 *    - Use useEffect to fetch on mount
 *    - Handle loading/error states
 *    - Store in local state
 *
 * 2. NO BUSINESS LOGIC
 *    - Just fetch + display
 *    - Backend validates everything
 *    - Frontend reflects backend state
 *
 * 3. ERROR HANDLING
 *    - Check response.success
 *    - Display error message from backend
 *    - Never assume operation succeeded
 *
 * 4. STATE TRANSITIONS
 *    - Disable buttons based on current state
 *    - Poll transaction status if needed
 *    - Show rollback if transaction failed
 *
 * 5. POLLING PATTERN
 *    - For async operations (transactions)
 *    - Check if still processing
 *    - Stop after timeout or success
 *    - Don't spin forever
 */
