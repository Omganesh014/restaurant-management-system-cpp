#pragma once
#include <string>
#include <vector>
#include <ctime>

struct SystemSnapshot {
    std::string snapshotId;
    std::time_t createdAt;
    std::string description;
    bool isValid;
    int customerCount;
    int orderCount;
    int menuItemCount;
};

class SnapshotManager {
public:
    static void createSnapshot(const std::string& desc, int customers, int orders, int items);
    static SystemSnapshot* getLatestSnapshot();
    static SystemSnapshot* getLastStableSnapshot();
    static bool restoreFromSnapshot(const std::string& id);
    static const std::vector<SystemSnapshot>& listSnapshots();
    static void pruneOldSnapshots(int keepCount);
    static int getSnapshotCount();
    static void markSnapshot(const std::string& id, bool valid);
};
