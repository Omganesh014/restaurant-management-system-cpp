/**
 * Health API Layer
 * 
 * System observability and readiness checks
 * 
 * PRINCIPLE:
 * Frontend can ask "Is backend ready?" at any time.
 * No polling—called on-demand when user needs diagnostics.
 */

import { apiClient } from './apiClient';
import { ApiResponse, HealthCheckDTO } from './types';

class HealthApi {
  /**
   * Get full system health
   * 
   * Backend checks:
   * - CSV storage accessible (write/read test)
   * - Snapshots available
   * - Event system operational
   * - Services initialized
   * - Memory usage estimation
   * 
   * Frontend displays:
   * - Status badge (HEALTHY/DEGRADED/UNHEALTHY)
   * - Component health grid
   * - Diagnostic message
   * - Refresh button
   */
  async getSystemHealth(): Promise<ApiResponse<HealthCheckDTO>> {
    return apiClient.request<HealthCheckDTO>('GET', '/api/health');
  }

  /**
   * Helper: Check if backend is ready for operations
   * Returns true only if HEALTHY
   */
  async isBackendReady(): Promise<boolean> {
    try {
      const response = await this.getSystemHealth();
      return response.success && response.data?.status === 'HEALTHY';
    } catch {
      return false;
    }
  }

  /**
   * Helper: Get human-readable status message
   */
  getStatusMessage(health: HealthCheckDTO): string {
    switch (health.status) {
      case 'HEALTHY':
        return 'All systems operational';
      case 'DEGRADED':
        return 'System running with reduced capacity';
      case 'UNHEALTHY':
        return 'System unavailable — contact support';
      default:
        return 'Unknown status';
    }
  }

  /**
   * Helper: Get component issues
   */
  getComponentStatus(health: HealthCheckDTO): Array<{
    name: string;
    ok: boolean;
  }> {
    return [
      { name: 'Storage (CSV)', ok: health.csvAccessible },
      { name: 'Snapshots', ok: health.snapshotAccessible },
      { name: 'Event System', ok: health.eventSystemOperational },
      { name: 'Services', ok: health.servicesInitialized },
    ];
  }
}

export const healthApi = new HealthApi();
