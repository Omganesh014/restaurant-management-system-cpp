#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include "Common.h"
#include <string>
#include <vector>
#include <functional>
#include <chrono>

/**
 * @class Transaction
 * @brief Lightweight ACID-like transaction system across services
 * 
 * Provides atomicity and consistency for multi-step operations:
 * - Order placement + inventory deduction + billing
 * - Refund + payment reversal
 * - Snapshot restore + state update
 * 
 * Usage:
 *   Transaction tx;
 *   tx.begin();
 *   tx.execute([](){ orderService.create(...); });
 *   tx.execute([](){ inventoryService.deduct(...); });
 *   tx.commit(); // or rollback()
 */

class Transaction {
public:
    enum class State { READY, ACTIVE, COMMITTED, ROLLED_BACK, FAILED };
    
    Transaction();
    
    // Transaction lifecycle
    void begin();
    void commit();
    void rollback();
    
    // Register work to do
    void execute(std::function<void()> operation, std::function<void()> rollbackOp = nullptr);
    
    // Query transaction state
    State getState() const;
    std::string getStateString() const;
    bool isSuccessful() const;
    const std::string& getErrorMessage() const;
    std::chrono::system_clock::time_point getStartTime() const;
    
    // Diagnostic info
    size_t getOperationCount() const;
    
private:
    struct Operation {
        std::function<void()> execute;
        std::function<void()> rollback;
        bool completed = false;
    };
    
    State state;
    std::vector<Operation> operations;
    std::string errorMessage;
    std::chrono::system_clock::time_point startTime;
    
    void applyRollbacks();
};

/**
 * @class TransactionManager
 * @brief Global transaction coordinator
 * 
 * Tracks active transactions and provides diagnostics.
 */
class TransactionManager {
public:
    static TransactionManager& instance();
    
    // Transaction management
    Transaction* createTransaction();
    void removeTransaction(Transaction* tx);
    
    // Diagnostics
    size_t getActiveTransactionCount() const;
    size_t getTotalCommittedTransactions() const;
    size_t getTotalFailedTransactions() const;
    
private:
    TransactionManager() = default;
    
    std::vector<Transaction*> activeTransactions;
    size_t totalCommitted = 0;
    size_t totalFailed = 0;
};

#endif
