/**
 * API Contract Types
 * 
 * These types define the FRONTEND↔BACKEND contract.
 * Backend must always return responses matching these shapes.
 * 
 * PRINCIPLE: Frontend trusts backend has enforced all business rules.
 *           Frontend's job is only to display and gather input.
 */

/**
 * Standard API response envelope
 * ALL backend responses must follow this format
 */
export interface ApiResponse<T = any> {
  success: boolean;
  data: T | null;
  error: ApiError | null;
  meta: {
    timestamp: string;
    transactionId: string;
  };
}

/**
 * Error details from backend
 */
export interface ApiError {
  code: string;
  message: string;
  details?: Record<string, any>;
}

/**
 * Order representation (backend FSM state)
 */
export interface OrderDTO {
  id: string;
  customerId: string;
  items: OrderItemDTO[];
  status: OrderStatus;
  total: number;
  createdAt: string;
  updatedAt: string;
}

export enum OrderStatus {
  CREATED = 'CREATED',
  CONFIRMED = 'CONFIRMED',
  PREPARING = 'PREPARING',
  READY = 'READY',
  SERVED = 'SERVED',
  CANCELLED = 'CANCELLED',
  FAILED = 'FAILED'
}

export interface OrderItemDTO {
  itemId: string;
  name: string;
  price: number;
  quantity: number;
  subtotal: number;
}

/**
 * Create order request
 */
export interface CreateOrderRequest {
  customerId: string;
  items: Array<{
    itemId: string;
    quantity: number;
  }>;
  idempotencyKey: string;
}

/**
 * Menu item (read-only from frontend)
 */
export interface MenuItemDTO {
  id: string;
  name: string;
  price: number;
  quantityAvailable: number;
  enabled: boolean;
}

/**
 * Transaction status from TransactionManager
 */
export interface TransactionDTO {
  id: string;
  status: TransactionStatus;
  operationCount: number;
  createdAt: string;
  completedAt?: string;
}

export enum TransactionStatus {
  READY = 'READY',
  ACTIVE = 'ACTIVE',
  COMMITTED = 'COMMITTED',
  ROLLED_BACK = 'ROLLED_BACK',
  FAILED = 'FAILED'
}

/**
 * System health from HealthService
 */
export interface HealthCheckDTO {
  status: HealthStatus;
  csvAccessible: boolean;
  snapshotAccessible: boolean;
  eventSystemOperational: boolean;
  servicesInitialized: boolean;
  estimatedMemoryMB: number;
  snapshotCount: number;
  diagnostics: string;
}

export enum HealthStatus {
  HEALTHY = 'HEALTHY',
  DEGRADED = 'DEGRADED',
  UNHEALTHY = 'UNHEALTHY'
}

/**
 * CQRS Query: Get active orders
 */
export interface GetActiveOrdersResponse {
  orders: OrderDTO[];
  count: number;
}

/**
 * CQRS Query: Get orders by customer
 */
export interface GetCustomerOrdersResponse {
  orders: OrderDTO[];
  customerId: string;
  count: number;
}

/**
 * CQRS Query: Get single order
 */
export interface GetOrderResponse {
  order: OrderDTO | null;
}

/**
 * Command response: Confirm order
 */
export interface ConfirmOrderResponse {
  orderId: string;
  newStatus: OrderStatus;
  transactionId: string;
}

/**
 * Command response: Cancel order
 */
export interface CancelOrderResponse {
  orderId: string;
  newStatus: OrderStatus;
  refundAmount: number;
  transactionId: string;
}
