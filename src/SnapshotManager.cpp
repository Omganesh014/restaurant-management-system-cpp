#include "SnapshotManager.h"
#include "Logger.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

static std::vector<SystemSnapshot> snapshots;

void SnapshotManager::createSnapshot(const std::string& desc, int customers, int orders, int items) {
    auto now = std::time(nullptr);
    auto tm = std::localtime(&now);
    std::ostringstream oss;
    oss << "snap_" << std::put_time(tm, "%Y%m%d_%H%M%S");
    
    SystemSnapshot snap;
    snap.snapshotId = oss.str();
    snap.createdAt = now;
    snap.description = desc;
    snap.isValid = true;
    snap.customerCount = customers;
    snap.orderCount = orders;
    snap.menuItemCount = items;
    
    snapshots.push_back(snap);
    Logger::log(LogLevel::INFO, "Snapshot: " + snap.snapshotId + " (" + desc + ")");
}

SystemSnapshot* SnapshotManager::getLatestSnapshot() {
    if (snapshots.empty()) return nullptr;
    return &snapshots.back();
}

SystemSnapshot* SnapshotManager::getLastStableSnapshot() {
    for (auto it = snapshots.rbegin(); it != snapshots.rend(); ++it) {
        if (it->isValid) return &(*it);
    }
    return nullptr;
}

bool SnapshotManager::restoreFromSnapshot(const std::string& id) {
    auto it = std::find_if(snapshots.begin(), snapshots.end(),
                          [&id](const SystemSnapshot& s) { return s.snapshotId == id; });
    if (it == snapshots.end()) return false;
    Logger::log(LogLevel::INFO, "Restored from: " + id);
    return true;
}

const std::vector<SystemSnapshot>& SnapshotManager::listSnapshots() {
    return snapshots;
}

void SnapshotManager::pruneOldSnapshots(int keepCount) {
    if ((int)snapshots.size() > keepCount) {
        snapshots.erase(snapshots.begin(), snapshots.begin() + snapshots.size() - keepCount);
        Logger::log(LogLevel::INFO, "Pruned snapshots");
    }
}

int SnapshotManager::getSnapshotCount() {
    return snapshots.size();
}

void SnapshotManager::markSnapshot(const std::string& id, bool valid) {
    auto it = std::find_if(snapshots.begin(), snapshots.end(),
                          [&id](const SystemSnapshot& s) { return s.snapshotId == id; });
    if (it != snapshots.end()) {
        it->isValid = valid;
        std::string status = valid ? "valid" : "invalid";
        Logger::log(LogLevel::INFO, "Marked snapshot " + status);
    }
}
