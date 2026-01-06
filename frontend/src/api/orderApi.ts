/**
 * Order API Layer
 * 
 * All order operations go through here.
 * 
 * CONTRACTS ENFORCED:
 * - Backend enforces FSM state transitions
 * - Backend enforces inventory availability
 * - Backend enforces idempotency
 * - Frontend just displays and gathers input
 */

import { apiClient } from './apiClient';
import {
  ApiResponse,
  OrderDTO,
  CreateOrderRequest,
  ConfirmOrderResponse,
  CancelOrderResponse,
  GetActiveOrdersResponse,
  GetCustomerOrdersResponse,
  GetOrderResponse,
} from './types';

class OrderApi {
  /**
   * Get all active orders
   * Backend decides what "active" means (CQRS Query)
   */
  async getActiveOrders(): Promise<ApiResponse<GetActiveOrdersResponse>> {
    return apiClient.request<GetActiveOrdersResponse>('GET', '/api/orders');
  }

  /**
   * Get orders for specific customer
   * Backend filters, frontend displays
   */
  async getCustomerOrders(
    customerId: string
  ): Promise<ApiResponse<GetCustomerOrdersResponse>> {
    return apiClient.request<GetCustomerOrdersResponse>(
      'GET',
      `/api/customers/${customerId}/orders`
    );
  }

  /**
   * Get single order details
   */
  async getOrder(orderId: string): Promise<ApiResponse<GetOrderResponse>> {
    return apiClient.request<GetOrderResponse>('GET', `/api/orders/${orderId}`);
  }

  /**
   * Create new order
   * 
   * Backend responsibility:
   * - Validate menu items exist
   * - Check inventory availability
   * - Calculate total
   * - Assign transaction ID
   * - Return FSM state (CREATED)
   * 
   * Frontend responsibility:
   * - Collect items from UI
   * - Show success/error
   * - Move to next view
   */
  async placeOrder(request: CreateOrderRequest): Promise<ApiResponse<OrderDTO>> {
    return apiClient.request<OrderDTO>('POST', '/api/orders', {
      customerId: request.customerId,
      items: request.items,
      idempotencyKey: request.idempotencyKey,
    });
  }

  /**
   * Confirm order (transition CREATED → CONFIRMED)
   * 
   * Backend responsibility:
   * - Validate current state is CREATED
   * - Reserve inventory
   * - Generate bill
   * - Wrap in Transaction (atomic multi-step)
   * - Return new state or rollback
   * 
   * Frontend responsibility:
   * - Disable button if order not in CREATED state
   * - Show loading during request
   * - Display error if backend rejects transition
   */
  async confirmOrder(orderId: string): Promise<ApiResponse<ConfirmOrderResponse>> {
    return apiClient.request<ConfirmOrderResponse>(
      'POST',
      `/api/orders/${orderId}/confirm`
    );
  }

  /**
   * Cancel order
   * 
   * Backend responsibility:
   * - Validate order exists
   * - Check if cancellable (not SERVED, etc.)
   * - Reverse inventory reservation
   * - Calculate refund
   * - Emit event
   * 
   * Frontend responsibility:
   * - Disable cancel button if not cancellable
   * - Show refund amount on success
   * - Reflect new status in UI
   */
  async cancelOrder(orderId: string): Promise<ApiResponse<CancelOrderResponse>> {
    return apiClient.request<CancelOrderResponse>(
      'POST',
      `/api/orders/${orderId}/cancel`
    );
  }

  /**
   * Mark order as served (kitchen → cashier transition)
   * Backend enforces state machine
   */
  async markAsServed(orderId: string): Promise<ApiResponse<OrderDTO>> {
    return apiClient.request<OrderDTO>(
      'POST',
      `/api/orders/${orderId}/serve`
    );
  }

  /**
   * Soft-delete order (GDPR/retention)
   * Backend handles soft-delete semantics
   */
  async softDeleteOrder(orderId: string): Promise<ApiResponse<void>> {
    return apiClient.request<void>(
      'DELETE',
      `/api/orders/${orderId}?soft=true`
    );
  }
}

export const orderApi = new OrderApi();
