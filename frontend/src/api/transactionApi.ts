/**
 * Transaction API Layer
 * 
 * CQRS + Transaction visibility
 * 
 * Frontend doesn't execute transactions—backend does.
 * Frontend asks "What's the status of transaction X?"
 */

import { apiClient } from './apiClient';
import { ApiResponse, TransactionDTO } from './types';

class TransactionApi {
  /**
   * Get transaction status
   * 
   * Backend tracks all transactions via TransactionManager.
   * Frontend can poll or subscribe to updates.
   * 
   * States:
   * - READY: Created, awaiting execution
   * - ACTIVE: In progress
   * - COMMITTED: All steps succeeded
   * - ROLLED_BACK: Explicit rollback (failure recovery)
   * - FAILED: Unrecoverable error
   */
  async getTransaction(transactionId: string): Promise<ApiResponse<TransactionDTO>> {
    return apiClient.request<TransactionDTO>(
      'GET',
      `/api/transactions/${transactionId}`
    );
  }

  /**
   * List recent transactions (last N)
   * Useful for audit/debug view
   */
  async getRecentTransactions(limit: number = 10): Promise<
    ApiResponse<{ transactions: TransactionDTO[]; count: number }>
  > {
    return apiClient.request(
      'GET',
      `/api/transactions?limit=${limit}`
    );
  }

  /**
   * Helper: Is transaction still processing?
   */
  isProcessing(transaction: TransactionDTO): boolean {
    return transaction.status === 'ACTIVE' || transaction.status === 'READY';
  }

  /**
   * Helper: Did transaction succeed?
   */
  isSuccessful(transaction: TransactionDTO): boolean {
    return transaction.status === 'COMMITTED';
  }

  /**
   * Helper: Did transaction fail?
   */
  isFailed(transaction: TransactionDTO): boolean {
    return transaction.status === 'FAILED' || transaction.status === 'ROLLED_BACK';
  }

  /**
   * Helper: Get human-readable status
   */
  getStatusMessage(transaction: TransactionDTO): string {
    const messages: Record<string, string> = {
      READY: 'Queued for execution',
      ACTIVE: 'Processing...',
      COMMITTED: 'Completed successfully',
      ROLLED_BACK: 'Rolled back — operation reversed',
      FAILED: 'Failed — contact support',
    };
    return messages[transaction.status] || 'Unknown status';
  }
}

export const transactionApi = new TransactionApi();
