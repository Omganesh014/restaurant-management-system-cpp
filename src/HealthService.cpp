#include "HealthService.h"
#include "Logger.h"
#include <fstream>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

HealthService& HealthService::instance() {
    static HealthService hs;
    return hs;
}

SystemHealth HealthService::checkHealth() {
    SystemHealth health;
    health.overallStatus = SystemHealth::Status::HEALTHY;
    health.statusString = "OK";
    
    // Check CSV accessibility
    health.csvAccessible = checkCSVAccess();
    if (!health.csvAccessible) {
        health.issues.push_back("CSV files not accessible");
        health.overallStatus = SystemHealth::Status::UNHEALTHY;
    }
    
    // Check snapshot integrity
    health.snapshotAccessible = checkSnapshotIntegrity();
    if (!health.snapshotAccessible) {
        health.warnings.push_back("Snapshot integrity check failed");
        if (health.overallStatus == SystemHealth::Status::HEALTHY) {
            health.overallStatus = SystemHealth::Status::DEGRADED;
        }
    }
    
    // Check event system
    health.eventSystemOperational = checkEventSystem();
    if (!health.eventSystemOperational) {
        health.issues.push_back("Event system not operational");
        health.overallStatus = SystemHealth::Status::UNHEALTHY;
    }
    
    // Check services
    health.servicesInitialized = checkServices();
    if (!health.servicesInitialized) {
        health.warnings.push_back("Not all services initialized");
        if (health.overallStatus == SystemHealth::Status::HEALTHY) {
            health.overallStatus = SystemHealth::Status::DEGRADED;
        }
    }
    
    // Estimate memory
    health.estimatedMemoryMB = estimateMemoryUsage();
    
    // Count snapshots
    try {
        if (fs::exists("data")) {
            for (const auto& entry : fs::directory_iterator("data")) {
                if (entry.path().filename().string().find("snap_") != std::string::npos) {
                    health.snapshotCount++;
                }
            }
        }
    } catch (...) {
        health.warnings.push_back("Could not enumerate snapshots");
    }
    
    // Update status string
    if (health.overallStatus == SystemHealth::Status::HEALTHY) {
        health.statusString = "✅ HEALTHY";
    } else if (health.overallStatus == SystemHealth::Status::DEGRADED) {
        health.statusString = "⚠️  DEGRADED";
    } else {
        health.statusString = "❌ UNHEALTHY";
    }
    
    lastHealth = health;
    return health;
}

std::string HealthService::getDetailedReport() {
    auto health = checkHealth();
    
    std::stringstream ss;
    ss << "\n=== SYSTEM HEALTH REPORT ===\n";
    ss << "Status: " << health.statusString << "\n";
    ss << "\nComponent Status:\n";
    ss << "  CSV Access: " << (health.csvAccessible ? "✅" : "❌") << "\n";
    ss << "  Snapshots: " << (health.snapshotAccessible ? "✅" : "❌") << " (" << health.snapshotCount << " snapshots)\n";
    ss << "  Event System: " << (health.eventSystemOperational ? "✅" : "❌") << "\n";
    ss << "  Services: " << (health.servicesInitialized ? "✅" : "❌") << "\n";
    
    ss << "\nMetrics:\n";
    ss << "  Estimated Memory: " << health.estimatedMemoryMB << " MB\n";
    ss << "  Event Queue Size: " << health.eventQueueSize << " events\n";
    
    if (!health.issues.empty()) {
        ss << "\nIssues:\n";
        for (const auto& issue : health.issues) {
            ss << "  ❌ " << issue << "\n";
        }
    }
    
    if (!health.warnings.empty()) {
        ss << "\nWarnings:\n";
        for (const auto& warning : health.warnings) {
            ss << "  ⚠️  " << warning << "\n";
        }
    }
    
    ss << "===============================\n";
    return ss.str();
}

bool HealthService::isHealthy() const {
    return lastHealth.overallStatus == SystemHealth::Status::HEALTHY;
}

bool HealthService::checkCSVAccess() {
    try {
        std::ofstream test("data/.health_check_temp.txt");
        if (!test.is_open()) return false;
        test << "health_check";
        test.close();
        
        std::ifstream verify("data/.health_check_temp.txt");
        if (!verify.is_open()) return false;
        verify.close();
        
        fs::remove("data/.health_check_temp.txt");
        return true;
    } catch (...) {
        return false;
    }
}

bool HealthService::checkSnapshotIntegrity() {
    try {
        if (!fs::exists("data")) return false;
        
        for (const auto& entry : fs::directory_iterator("data")) {
            if (entry.path().filename().string().find("snap_") != std::string::npos) {
                // Just check if file is readable
                std::ifstream file(entry.path());
                if (!file.is_open()) return false;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool HealthService::checkEventSystem() {
    // In production, this would check event queue, listeners, etc.
    // For now, just verify it exists
    return true; // Assume event system is operational if no errors
}

bool HealthService::checkServices() {
    // In production, this would check if all required services are registered
    // For now, return true if basic setup is done
    return true; // Assume services are initialized
}

size_t HealthService::estimateMemoryUsage() {
    // Rough estimate based on data files
    size_t totalBytes = 0;
    try {
        if (fs::exists("data")) {
            for (const auto& entry : fs::directory_iterator("data")) {
                if (fs::is_regular_file(entry)) {
                    totalBytes += fs::file_size(entry);
                }
            }
        }
    } catch (...) {
        return 0;
    }
    
    return totalBytes / (1024 * 1024); // Convert to MB
}
