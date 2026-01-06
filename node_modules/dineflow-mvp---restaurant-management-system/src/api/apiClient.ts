/**
 * Backend API Client (Transport Abstraction)
 * 
 * SINGLE POINT OF CONTACT for backend communication.
 * 
 * Responsibilities:
 * - Make HTTP/file requests to backend
 * - Deserialize responses
 * - Handle transport-level errors
 * - Retry logic (if needed)
 * 
 * NOT responsible for:
 * - Business logic validation
 * - State management
 * - Cache invalidation
 */

import { ApiResponse, ApiError } from './types';

/**
 * Backend transport configuration
 * Can be swapped for file-based, mock, or real HTTP
 */
interface BackendConfig {
  baseUrl: string;
  timeout: number;
  mockMode: boolean;
}

class ApiClient {
  private config: BackendConfig;

  constructor(config: Partial<BackendConfig> = {}) {
    this.config = {
      baseUrl: config.baseUrl || 'http://localhost:3001',
      timeout: config.timeout || 5000,
      mockMode: config.mockMode ?? true, // Default to mock for development
    };
  }

  /**
   * Core request method
   * Wraps all transport logic
   */
  async request<T>(
    method: string,
    endpoint: string,
    body?: any
  ): Promise<ApiResponse<T>> {
    try {
      // In mock mode, read from JSON files in data/
      if (this.config.mockMode) {
        return await this.mockRequest<T>(method, endpoint, body);
      }

      // Real HTTP request
      const url = `${this.config.baseUrl}${endpoint}`;
      const options: RequestInit = {
        method,
        headers: {
          'Content-Type': 'application/json',
        },
      };

      if (body) {
        options.body = JSON.stringify(body);
      }

      const controller = new AbortController();
      const timeoutId = setTimeout(() => controller.abort(), this.config.timeout);

      const response = await fetch(url, {
        ...options,
        signal: controller.signal,
      });

      clearTimeout(timeoutId);

      if (!response.ok) {
        throw new Error(`HTTP ${response.status}: ${response.statusText}`);
      }

      return await response.json();
    } catch (error) {
      return this.errorResponse<T>(error);
    }
  }

  /**
   * Mock request (for development without backend server)
   */
  private async mockRequest<T>(
    method: string,
    endpoint: string,
    body?: any
  ): Promise<ApiResponse<T>> {
    // Simulate network delay
    await new Promise((resolve) => setTimeout(resolve, 100));

    // Route to appropriate mock response
    const key = `${method.toUpperCase()} ${endpoint}`;

    const mocks: Record<string, any> = {
      'GET /api/health': {
        success: true,
        data: {
          status: 'HEALTHY',
          csvAccessible: true,
          snapshotAccessible: true,
          eventSystemOperational: true,
          servicesInitialized: true,
          estimatedMemoryMB: 45,
          snapshotCount: 3,
          diagnostics: 'All systems operational',
        },
        error: null,
        meta: {
          timestamp: new Date().toISOString(),
          transactionId: this.generateId(),
        },
      },

      'GET /api/orders': {
        success: true,
        data: {
          orders: [
            {
              id: 'ORD-001',
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
              createdAt: new Date(Date.now() - 300000).toISOString(),
              updatedAt: new Date(Date.now() - 60000).toISOString(),
            },
          ],
          count: 1,
        },
        error: null,
        meta: {
          timestamp: new Date().toISOString(),
          transactionId: this.generateId(),
        },
      },

      'POST /api/orders': {
        success: true,
        data: {
          id: 'ORD-002',
          customerId: body?.customerId || 'CUST-001',
          items: body?.items || [],
          status: 'CREATED',
          total: body?.total || 0,
          createdAt: new Date().toISOString(),
          updatedAt: new Date().toISOString(),
        },
        error: null,
        meta: {
          timestamp: new Date().toISOString(),
          transactionId: this.generateId(),
        },
      },

      'POST /api/transactions': {
        success: true,
        data: {
          id: `TXN-${this.generateId()}`,
          status: 'COMMITTED',
          operationCount: 3,
          createdAt: new Date(Date.now() - 5000).toISOString(),
          completedAt: new Date().toISOString(),
        },
        error: null,
        meta: {
          timestamp: new Date().toISOString(),
          transactionId: this.generateId(),
        },
      },
    };

    const response = mocks[key];
    if (!response) {
      return this.errorResponse<T>(new Error(`No mock for ${key}`));
    }

    return response;
  }

  /**
   * Generate standard error response
   */
  private errorResponse<T>(error: any): ApiResponse<T> {
    const message =
      error instanceof Error ? error.message : 'Unknown error';

    return {
      success: false,
      data: null,
      error: {
        code: 'TRANSPORT_ERROR',
        message,
      },
      meta: {
        timestamp: new Date().toISOString(),
        transactionId: this.generateId(),
      },
    };
  }

  /**
   * Utility: generate unique ID
   */
  private generateId(): string {
    return `${Date.now()}-${Math.random().toString(36).substr(2, 9)}`;
  }

  /**
   * Change backend mode (mock ↔ real)
   */
  setMockMode(enabled: boolean): void {
    this.config.mockMode = enabled;
  }
}

// Singleton instance
export const apiClient = new ApiClient({
  mockMode: true, // Development default
});
