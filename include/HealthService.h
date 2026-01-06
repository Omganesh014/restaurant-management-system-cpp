#ifndef HEALTH_SERVICE_H
#define HEALTH_SERVICE_H

#include "Common.h"
#include <string>
#include <vector>

/**
 * @class SystemHealth
 * @brief Diagnostic health status of the backend system
 */
struct SystemHealth {
    enum class Status { HEALTHY, DEGRADED, UNHEALTHY };
    
    Status overallStatus;
    std::string statusString;
    std::vector<std::string> issues;
    std::vector<std::string> warnings;
    
    // Component health
    bool csvAccessible = false;
    bool snapshotAccessible = false;
    bool eventSystemOperational = false;
    bool servicesInitialized = false;
    
    // Metrics
    size_t estimatedMemoryMB = 0;
    size_t snapshotCount = 0;
    size_t eventQueueSize = 0;
};

/**
 * @class HealthService
 * @brief Self-diagnostic backend health monitoring
 * 
 * Checks:
 * - CSV file accessibility
 * - Snapshot integrity
 * - Event system status
 * - Memory estimates
 * - Service availability
 */
class HealthService {
public:
    static HealthService& instance();
    
    // Health check
    SystemHealth checkHealth();
    
    // Detailed diagnostics
    std::string getDetailedReport();
    bool isHealthy() const;
    
private:
    HealthService() = default;
    SystemHealth lastHealth;
    
    // Component checks
    bool checkCSVAccess();
    bool checkSnapshotIntegrity();
    bool checkEventSystem();
    bool checkServices();
    size_t estimateMemoryUsage();
};

#endif
